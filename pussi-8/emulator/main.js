import specs from "./specs.js";
import State from "./state.js";
import UiManager from "./ui.js";

const state = new State(specs);
const ui = new UiManager(state);
ui.display();
