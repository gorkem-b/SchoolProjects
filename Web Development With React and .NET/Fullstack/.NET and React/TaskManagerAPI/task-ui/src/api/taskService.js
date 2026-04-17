import api from "./axiosConfig";

export const getTasks = () => api.get("/tasks");
export const createTask = (payload) => api.post("/tasks", payload);
export const updateTask = (id, payload) => api.put(`/tasks/${id}`, payload);
export const deleteTask = (id) => api.delete(`/tasks/${id}`);