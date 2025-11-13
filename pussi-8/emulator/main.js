import State from "./state.js";
import UiManager from "./ui.js";

const state = new State();
const ui = new UiManager(state);
ui.display();
