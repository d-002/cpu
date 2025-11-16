export function setupUserChanges(state, ui) {
    document.body.addEventListener("change", evt => {
        const data = evt.target.parentNode;

        if (!data.classList.contains("data"))
            return;

        const value = evt.target.value.match(/[0-9a-fA-F]*/g).join("");
        state.updateState(data.getAttribute("path"), value);
        ui.display();
    });
}
