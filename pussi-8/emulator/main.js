import State from "./state.js";
import UiManager from "./ui.js";
import { setupUserChanges } from "./user.js";
import { setupRun } from "./run.js";

const state = new State();
const ui = new UiManager(state);

setupUserChanges(state, ui);
setupRun(state, ui);

ui.display();
