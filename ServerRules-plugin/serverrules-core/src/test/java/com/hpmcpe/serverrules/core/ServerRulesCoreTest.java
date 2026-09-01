package com.hpmcpe.serverrules.core;

import com.hpmcpe.serverrules.api.model.Rule;
import com.hpmcpe.serverrules.api.model.RuleCategory;
import com.hpmcpe.serverrules.api.model.RuleSeverity;
import com.hpmcpe.serverrules.core.manager.DefaultAcceptanceManager;
import com.hpmcpe.serverrules.core.manager.DefaultCategoryManager;
import com.hpmcpe.serverrules.core.manager.DefaultRuleManager;
import com.hpmcpe.serverrules.api.model.PlayerAcceptance;
import org.junit.jupiter.api.*;

import java.util.*;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Unit tests for core rule management logic.
 */
class ServerRulesCoreTest {

    private DefaultRuleManager ruleManager;
    private DefaultCategoryManager categoryManager;
    private DefaultAcceptanceManager acceptanceManager;

    private final int[] currentVersion = {1};

    @BeforeEach
    void setUp() {
        ruleManager = new DefaultRuleManager(null);
        categoryManager = new DefaultCategoryManager(null);
        acceptanceManager = new DefaultAcceptanceManager(
                () -> currentVersion[0],
                new NoOpStorage()
        );

        // Load sample data
        List<RuleCategory> cats = List.of(
                new RuleCategory.Builder("general").displayName("General").order(0).build(),
                new RuleCategory.Builder("chat").displayName("Chat").order(1).build()
        );
        categoryManager.loadCategories(cats);

        List<Rule> rules = List.of(
                new Rule.Builder("general-1", "general").text("Respect all players.").order(0).build(),
                new Rule.Builder("general-2", "general").text("No griefing.").severity(RuleSeverity.HIGH).order(1).build(),
                new Rule.Builder("chat-1", "chat").text("No spam.").order(0).build(),
                new Rule.Builder("chat-2", "chat").text("No advertising.").enabled(false).order(1).build()
        );
        ruleManager.loadRules(rules);
    }

    // -------------------------------------------------------------------------
    // Rule CRUD
    // -------------------------------------------------------------------------

    @Test
    void testGetAllRules_returnsFour() {
        assertEquals(4, ruleManager.getTotalRuleCount());
    }

    @Test
    void testGetEnabledRules_returnsThree() {
        assertEquals(3, ruleManager.getEnabledRuleCount());
    }

    @Test
    void testGetRuleById_found() {
        assertTrue(ruleManager.getRule("general-1").isPresent());
    }

    @Test
    void testGetRuleById_notFound() {
        assertTrue(ruleManager.getRule("nonexistent").isEmpty());
    }

    @Test
    void testGetRulesByCategory() {
        List<Rule> generalRules = ruleManager.getRulesByCategory("general");
        assertEquals(2, generalRules.size());
    }

    @Test
    void testGetEnabledRulesByCategory_filtersDisabled() {
        List<Rule> chatEnabled = ruleManager.getEnabledRulesByCategory("chat");
        assertEquals(1, chatEnabled.size());
        assertEquals("chat-1", chatEnabled.get(0).getId());
    }

    @Test
    void testAddRule_succeeds() {
        Rule newRule = new Rule.Builder("general-3", "general").text("Follow staff instructions.").build();
        ruleManager.addRule(newRule);
        assertEquals(5, ruleManager.getTotalRuleCount());
        assertTrue(ruleManager.existsRule("general-3"));
    }

    @Test
    void testAddRule_duplicateId_throwsException() {
        Rule duplicate = new Rule.Builder("general-1", "general").text("Duplicate.").build();
        assertThrows(IllegalArgumentException.class, () -> ruleManager.addRule(duplicate));
    }

    @Test
    void testRemoveRule_returnsTrue() {
        assertTrue(ruleManager.removeRule("chat-2"));
        assertEquals(3, ruleManager.getTotalRuleCount());
    }

    @Test
    void testRemoveRule_notFound_returnsFalse() {
        assertFalse(ruleManager.removeRule("does-not-exist"));
    }

    @Test
    void testUpdateRule_updatesText() {
        Rule updated = ruleManager.getRule("general-1").get().toBuilder()
                .text("Updated text.")
                .build();
        ruleManager.updateRule(updated);
        assertEquals("Updated text.", ruleManager.getRule("general-1").get().getText());
    }

    @Test
    void testUpdateRule_notExisting_throwsException() {
        Rule ghost = new Rule.Builder("ghost-rule", "general").text("Ghost.").build();
        assertThrows(IllegalArgumentException.class, () -> ruleManager.updateRule(ghost));
    }

    // -------------------------------------------------------------------------
    // Search
    // -------------------------------------------------------------------------

    @Test
    void testSearchRules_foundByText() {
        List<Rule> results = ruleManager.searchRules("spam");
        assertEquals(1, results.size());
        assertEquals("chat-1", results.get(0).getId());
    }

    @Test
    void testSearchRules_caseInsensitive() {
        List<Rule> results = ruleManager.searchRules("RESPECT");
        assertEquals(1, results.size());
    }

    @Test
    void testSearchRules_noMatch_returnsEmpty() {
        List<Rule> results = ruleManager.searchRules("xyznonexistent");
        assertTrue(results.isEmpty());
    }

    @Test
    void testSearchRules_nullKeyword_returnsEmpty() {
        List<Rule> results = ruleManager.searchRules(null);
        assertTrue(results.isEmpty());
    }

    // -------------------------------------------------------------------------
    // Categories
    // -------------------------------------------------------------------------

    @Test
    void testGetAllCategories_sorted() {
        List<RuleCategory> cats = categoryManager.getAllCategories();
        assertEquals(2, cats.size());
        assertEquals("general", cats.get(0).getId());
        assertEquals("chat", cats.get(1).getId());
    }

    @Test
    void testAddCategory_duplicateId_throws() {
        RuleCategory dup = new RuleCategory.Builder("general").displayName("Dup").build();
        assertThrows(IllegalArgumentException.class, () -> categoryManager.addCategory(dup));
    }

    @Test
    void testRemoveCategory() {
        assertTrue(categoryManager.removeCategory("chat"));
        assertEquals(1, categoryManager.getCategoryCount());
    }

    // -------------------------------------------------------------------------
    // Acceptance
    // -------------------------------------------------------------------------

    @Test
    void testAcceptance_notAcceptedByDefault() {
        UUID uuid = UUID.randomUUID();
        assertFalse(acceptanceManager.hasAccepted(uuid));
        assertFalse(acceptanceManager.hasEverAccepted(uuid));
    }

    @Test
    void testAcceptance_acceptAndCheck() {
        UUID uuid = UUID.randomUUID();
        acceptanceManager.setAccepted(uuid, "TestPlayer");
        assertTrue(acceptanceManager.hasAccepted(uuid));
        assertTrue(acceptanceManager.hasEverAccepted(uuid));
    }

    @Test
    void testAcceptance_versionUpgrade_invalidatesOldAcceptance() {
        UUID uuid = UUID.randomUUID();
        currentVersion[0] = 1;
        acceptanceManager.setAccepted(uuid, "TestPlayer");
        assertTrue(acceptanceManager.hasAccepted(uuid));

        // Bump version
        currentVersion[0] = 2;
        assertFalse(acceptanceManager.hasAccepted(uuid));
        assertTrue(acceptanceManager.hasEverAccepted(uuid)); // still has old record
    }

    @Test
    void testAcceptance_reset() {
        UUID uuid = UUID.randomUUID();
        acceptanceManager.setAccepted(uuid, "TestPlayer");
        assertTrue(acceptanceManager.resetAcceptance(uuid));
        assertFalse(acceptanceManager.hasAccepted(uuid));
    }

    @Test
    void testAcceptance_resetNonExisting_returnsFalse() {
        assertFalse(acceptanceManager.resetAcceptance(UUID.randomUUID()));
    }

    @Test
    void testAcceptance_getAcceptedCount() {
        UUID a = UUID.randomUUID();
        UUID b = UUID.randomUUID();
        currentVersion[0] = 1;
        acceptanceManager.setAccepted(a, "PlayerA");
        acceptanceManager.setAccepted(b, "PlayerB");
        assertEquals(2, acceptanceManager.getAcceptedCount());
    }

    // -------------------------------------------------------------------------
    // Rule model
    // -------------------------------------------------------------------------

    @Test
    void testRule_builderMissingText_throws() {
        assertThrows(IllegalStateException.class, () ->
                new Rule.Builder("no-text", "general").build());
    }

    @Test
    void testRule_severity_defaultMedium() {
        Rule rule = new Rule.Builder("r1", "general").text("Some rule.").build();
        assertEquals(RuleSeverity.MEDIUM, rule.getSeverity());
    }

    @Test
    void testRuleSeverity_fromString_invalidFallback() {
        assertEquals(RuleSeverity.MEDIUM, RuleSeverity.fromString("INVALID"));
        assertEquals(RuleSeverity.MEDIUM, RuleSeverity.fromString(null));
    }

    @Test
    void testRuleSeverity_fromString_caseInsensitive() {
        assertEquals(RuleSeverity.HIGH, RuleSeverity.fromString("high"));
        assertEquals(RuleSeverity.CRITICAL, RuleSeverity.fromString("CRITICAL"));
    }

    @Test
    void testRule_equalsById() {
        Rule r1 = new Rule.Builder("same-id", "general").text("Text A.").build();
        Rule r2 = new Rule.Builder("same-id", "chat").text("Text B.").build();
        assertEquals(r1, r2); // equality is by ID only
    }

    // -------------------------------------------------------------------------
    // No-op storage for tests
    // -------------------------------------------------------------------------

    private static class NoOpStorage implements DefaultAcceptanceManager.StorageDelegate {
        @Override public void save(Map<UUID, PlayerAcceptance> data) {}
        @Override public Map<UUID, PlayerAcceptance> load() { return new HashMap<>(); }
    }
}
