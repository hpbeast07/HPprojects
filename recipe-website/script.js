const recipeDetails = {
    smoothie: {
        title: "Vegan Smoothie Bowl",
        category: "Breakfast · Vegan",
        description: "A cool, creamy bowl layered with tropical fruit, coconut, and a handful of granola for the perfect morning crunch.",
        image: "images/Untitled.jpg",
        alt: "Vegan smoothie bowl with fresh fruit and granola",
        meta: ["10 min", "Easy", "350 kcal"],
        ingredients: ["Frozen banana", "Mango chunks", "Coconut yogurt", "Oat milk", "Granola", "Fresh berries"]
    },
    pancakes: {
        title: "Gluten-Free Pancakes",
        category: "Breakfast · Gluten-free",
        description: "Tender, golden pancakes that flip beautifully and taste every bit as comforting as the classic stack.",
        image: "images/pancakes.jpg",
        alt: "Stack of golden gluten-free pancakes",
        meta: ["20 min", "Easy", "420 kcal"],
        ingredients: ["Gluten-free flour", "Baking powder", "Eggs", "Oat milk", "Maple syrup", "Vanilla"]
    },
    cake: {
        title: "Dark Chocolate Cake",
        category: "Dessert · Dairy-free",
        description: "An intensely chocolatey bake with a soft, fudgy center—made without a drop of dairy.",
        image: "images/cake.jpg",
        alt: "Slice of rich dark chocolate cake",
        meta: ["45 min", "Medium", "510 kcal"],
        ingredients: ["Dark cocoa", "Plain flour", "Brown sugar", "Olive oil", "Espresso", "Dark chocolate"]
    },
    carbonara: {
        title: "Spaghetti Carbonara",
        category: "Italian · Dinner",
        description: "A glossy, peppery pasta classic where egg yolks and parmesan make an irresistibly silky sauce.",
        image: "images/Spaghetti-Carbonara.jpg",
        alt: "Creamy spaghetti carbonara topped with parmesan",
        meta: ["25 min", "Medium", "640 kcal"],
        ingredients: ["Spaghetti", "Egg yolks", "Parmesan", "Pancetta", "Black pepper", "Pecorino"]
    }
};

const searchInput = document.querySelector("#recipe-search");
const searchForm = document.querySelector("#recipe-search-form");
const clearSearchButton = document.querySelector("#clear-search");
const recipeCards = [...document.querySelectorAll(".recipe-card")];
const filterButtons = [...document.querySelectorAll(".filter-button")];
const filterStatus = document.querySelector("#filter-status");
const emptyState = document.querySelector("#empty-state");
const recipeSection = document.querySelector("#recipes");
const dialog = document.querySelector("#recipe-dialog");
const toast = document.querySelector("#toast");

let activeFilter = "all";
let toastTimeout;

function showToast(message) {
    window.clearTimeout(toastTimeout);
    toast.textContent = message;
    toast.classList.add("is-visible");
    toastTimeout = window.setTimeout(() => toast.classList.remove("is-visible"), 2800);
}

function updateClearButton() {
    clearSearchButton.hidden = searchInput.value.trim().length === 0;
}

function updateRecipeList() {
    const searchTerm = searchInput.value.trim().toLowerCase();
    let visibleCount = 0;

    recipeCards.forEach((card) => {
        const matchesFilter = activeFilter === "all" || card.dataset.categories.split(" ").includes(activeFilter);
        const matchesSearch = card.dataset.search.includes(searchTerm);
        const isVisible = matchesFilter && matchesSearch;

        card.classList.toggle("is-hidden", !isVisible);
        card.setAttribute("aria-hidden", String(!isVisible));
        if (isVisible) visibleCount += 1;
    });

    emptyState.hidden = visibleCount !== 0;
    filterStatus.textContent = visibleCount
        ? `Showing ${visibleCount} ${visibleCount === 1 ? "recipe" : "recipes"}`
        : "No recipes found";
    updateClearButton();
}

function selectFilter(filter) {
    activeFilter = filter;
    filterButtons.forEach((button) => {
        const selected = button.dataset.filter === filter;
        button.classList.toggle("is-selected", selected);
        button.setAttribute("aria-pressed", String(selected));
    });
    updateRecipeList();
}

filterButtons.forEach((button) => {
    button.addEventListener("click", () => selectFilter(button.dataset.filter));
});

searchInput.addEventListener("input", updateRecipeList);

searchForm.addEventListener("submit", (event) => {
    event.preventDefault();
    updateRecipeList();
    recipeSection.scrollIntoView({ behavior: "smooth", block: "start" });
});

clearSearchButton.addEventListener("click", () => {
    searchInput.value = "";
    searchInput.focus();
    updateRecipeList();
});

document.querySelectorAll("[data-quick-search]").forEach((button) => {
    button.addEventListener("click", () => {
        searchInput.value = button.dataset.quickSearch;
        selectFilter("all");
        recipeSection.scrollIntoView({ behavior: "smooth", block: "start" });
    });
});

document.querySelector("#reset-filters").addEventListener("click", () => {
    searchInput.value = "";
    selectFilter("all");
});

document.querySelectorAll("[data-scroll-recipes]").forEach((button) => {
    button.addEventListener("click", () => recipeSection.scrollIntoView({ behavior: "smooth", block: "start" }));
});

document.querySelectorAll(".save-button").forEach((button) => {
    button.addEventListener("click", () => {
        const saved = button.classList.toggle("is-saved");
        button.setAttribute("aria-pressed", String(saved));
        const recipeTitle = button.closest(".recipe-card").querySelector("h3").textContent;
        showToast(saved ? `${recipeTitle} saved to your favourites.` : `${recipeTitle} removed from favourites.`);
    });
});

function openRecipe(recipeId) {
    const recipe = recipeDetails[recipeId];
    if (!recipe) return;

    document.querySelector("#dialog-image").src = recipe.image;
    document.querySelector("#dialog-image").alt = recipe.alt;
    document.querySelector("#dialog-title").textContent = recipe.title;
    document.querySelector("#dialog-category").textContent = recipe.category;
    document.querySelector("#dialog-description").textContent = recipe.description;
    document.querySelector("#dialog-meta").innerHTML = recipe.meta.map((item) => `<span>${item}</span>`).join("");
    document.querySelector("#dialog-ingredients").innerHTML = recipe.ingredients.map((item) => `<li>${item}</li>`).join("");

    if (typeof dialog.showModal === "function") dialog.showModal();
    else dialog.setAttribute("open", "");
}

document.querySelectorAll("[data-open-recipe]").forEach((button) => {
    button.addEventListener("click", () => openRecipe(button.dataset.openRecipe));
});

function closeDialog() {
    if (typeof dialog.close === "function") dialog.close();
    else dialog.removeAttribute("open");
}

document.querySelectorAll("[data-close-dialog]").forEach((button) => button.addEventListener("click", closeDialog));
dialog.addEventListener("click", (event) => { if (event.target === dialog) closeDialog(); });

document.querySelector("#newsletter-form").addEventListener("submit", (event) => {
    event.preventDefault();
    const emailInput = document.querySelector("#newsletter-email");
    if (!emailInput.checkValidity()) {
        emailInput.reportValidity();
        return;
    }
    showToast("You’re on the list—your next menu is coming soon.");
    event.currentTarget.reset();
});

updateRecipeList();