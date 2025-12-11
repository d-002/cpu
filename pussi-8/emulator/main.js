import State from "./state.js";
import UiManager from "./ui.js";
import { setupImportExport } from "./import-export.js";
import { setupUserChanges } from "./user.js";

const state = new State();
const ui = new UiManager(state);

setupUserChanges(state, ui);
setupImportExport(state, ui);

ui.display();
