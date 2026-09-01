package com.hpmcpe.serverrules.core.config;

import com.hpmcpe.serverrules.api.model.Rule;
import com.hpmcpe.serverrules.api.model.RuleCategory;
import com.hpmcpe.serverrules.api.model.RuleSeverity;
import com.hpmcpe.serverrules.core.util.SRLogger;

import java.io.IOException;
import java.nio.file.Path;
import java.util.*;

/**
 * Loads {@link Rule} and {@link RuleCategory} objects from {@code rules.yml}.
 *
 * <p>Expected YAML structure:</p>
 * <pre>
 * rules-version: 1
 *
 * categories:
 *   general:
 *     display-name: "&6General"
 *     description: "Basic rules for all players."
 *     icon: "BOOK"
 *     icon-data: 0
 *     order: 1
 *     enabled: true
 *     rules:
 *       - id: "general-respect"
 *         text: "&fRespect all players and staff."
 *         description: "Treat others how you want to be treated."
 *         severity: HIGH
 *         enabled: true
 * </pre>
 */
public class RulesLoader {

    private final Path rulesFile;

    public RulesLoader(Path rulesFile) {
        this.rulesFile = rulesFile;
    }

    /**
     * Load and parse rules.yml.
     *
     * @return the parsed result
     */
    public LoadResult load() {
        if (!rulesFile.toFile().exists()) {
            SRLogger.warning("rules.yml not found at: " + rulesFile);
            return LoadResult.empty();
        }

        YamlConfig config;
        try {
            config = YamlConfig.load(rulesFile);
        } catch (IOException e) {
            SRLogger.severe("Failed to load rules.yml: " + e.getMessage(), e);
            return LoadResult.empty();
        }

        int rulesVersion = config.getInt("rules-version", 1);
        List<RuleCategory> categories = new ArrayList<>();
        List<Rule> rules = new ArrayList<>();
        Set<String> seenIds = new HashSet<>();

        Map<String, Object> categoriesSection = config.getSection("categories");
        int catOrder = 0;
        for (Map.Entry<String, Object> entry : categoriesSection.entrySet()) {
            String catId = entry.getKey();
            if (!(entry.getValue() instanceof Map<?, ?> catMap)) {
                SRLogger.warning("Category '" + catId + "' has invalid format — skipping.");
                continue;
            }

            @SuppressWarnings("unchecked")
            Map<String, Object> catData = (Map<String, Object>) catMap;
            YamlConfig catConfig = new YamlConfig.SectionView(catData);

            RuleCategory category = new RuleCategory.Builder(catId)
                    .displayName(catConfig.getString("display-name", catId))
                    .description(catConfig.getString("description", ""))
                    .icon(catConfig.getString("icon", "BOOK"))
                    .iconData(catConfig.getInt("icon-data", 0))
                    .order(catConfig.getInt("order", catOrder++))
                    .enabled(catConfig.getBoolean("enabled", true))
                    .build();
            categories.add(category);

            // Parse rules list
            Object rawRules = catData.get("rules");
            if (!(rawRules instanceof List<?> ruleList)) continue;

            int ruleOrder = 0;
            for (Object rawRule : ruleList) {
                if (!(rawRule instanceof Map<?, ?> ruleMap)) {
                    SRLogger.warning("A rule entry in category '" + catId + "' is malformed — skipping.");
                    continue;
                }
                @SuppressWarnings("unchecked")
                Map<String, Object> ruleData = (Map<String, Object>) ruleMap;
                YamlConfig ruleConfig = new YamlConfig.SectionView(ruleData);

                String id = ruleConfig.getString("id", null);
                String text = ruleConfig.getString("text", null);

                if (id == null || id.isBlank()) {
                    SRLogger.warning("A rule in category '" + catId + "' is missing 'id' — skipping.");
                    continue;
                }
                if (text == null || text.isBlank()) {
                    SRLogger.warning("Rule '" + id + "' is missing required field 'text' — skipping.");
                    continue;
                }
                if (seenIds.contains(id)) {
                    SRLogger.warning("Duplicate rule ID '" + id + "' — skipping duplicate.");
                    continue;
                }
                seenIds.add(id);

                try {
                    Rule rule = new Rule.Builder(id, catId)
                            .text(text)
                            .description(ruleConfig.getString("description", ""))
                            .severity(RuleSeverity.fromString(ruleConfig.getString("severity", "MEDIUM")))
                            .enabled(ruleConfig.getBoolean("enabled", true))
                            .order(ruleConfig.getInt("order", ruleOrder++))
                            .build();
                    rules.add(rule);
                } catch (Exception e) {
                    SRLogger.warning("Could not load rule '" + id + "': " + e.getMessage());
                }
            }
        }

        SRLogger.info("Loaded " + rules.size() + " rules in " + categories.size() + " categories (version " + rulesVersion + ").");
        return new LoadResult(categories, rules, rulesVersion);
    }

    // -------------------------------------------------------------------------
    // Result
    // -------------------------------------------------------------------------

    public static final class LoadResult {
        private final List<RuleCategory> categories;
        private final List<Rule> rules;
        private final int rulesVersion;

        LoadResult(List<RuleCategory> categories, List<Rule> rules, int rulesVersion) {
            this.categories = Collections.unmodifiableList(categories);
            this.rules = Collections.unmodifiableList(rules);
            this.rulesVersion = rulesVersion;
        }

        public List<RuleCategory> getCategories() { return categories; }
        public List<Rule> getRules() { return rules; }
        public int getRulesVersion() { return rulesVersion; }

        static LoadResult empty() {
            return new LoadResult(new ArrayList<>(), new ArrayList<>(), 1);
        }
    }
}
