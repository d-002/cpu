import State from "./state.js";
import UiManager from "./ui.js";
import { setupUserChanges } from "./user.js";

const state = new State();
const ui = new UiManager(state);

setupUserChanges(state, ui);

ui.display();
