import { create } from 'zustand';
import client, { API_BASE_URL } from '../api/client.js';

const sortByDueDate = (a, b) => new Date(a.dueDate) - new Date(b.dueDate);

const cleanParams = (filters = {}) =>
  Object.fromEntries(
    Object.entries(filters).filter(([, value]) => value !== undefined && value !== null && value !== ''),
  );

const getErrorMessage = (error) => {
  if (!error?.response) {
    return `Cannot reach the API at ${API_BASE_URL}. Make sure the backend is running and reachable from this browser.`;
  }

  if (error.response.status >= 500) {
    return error.response.data?.message || 'API request failed. Check backend logs and database migrations.';
  }

  return error.response.data?.message || error.message || 'An unexpected error occurred.';
};

export const useTaskStore = create((set) => ({
  tasks: [],
  isLoading: false,
  error: null,

  fetchTasks: async (filters = {}) => {
    set({ isLoading: true, error: null });

    try {
      const response = await client.get('/tasks', { params: cleanParams(filters) });
      set({ tasks: response.data, isLoading: false });
    } catch (error) {
      set({ error: getErrorMessage(error), isLoading: false });
    }
  },

  createTask: async (task) => {
    try {
      const response = await client.post('/tasks', task);
      set((state) => ({
        tasks: [...state.tasks, response.data].sort(sortByDueDate),
        error: null,
      }));
      return response.data;
    } catch (error) {
      set({ error: getErrorMessage(error) });
      throw error;
    }
  },

  updateTask: async (id, task) => {
    try {
      const response = await client.put(`/tasks/${id}`, task);
      set((state) => ({
        tasks: state.tasks
          .map((item) => (item.id === id ? response.data : item))
          .sort(sortByDueDate),
        error: null,
      }));
      return response.data;
    } catch (error) {
      set({ error: getErrorMessage(error) });
      throw error;
    }
  },

  deleteTask: async (id) => {
    try {
      await client.delete(`/tasks/${id}`);
      set((state) => ({
        tasks: state.tasks.filter((task) => task.id !== id),
        error: null,
      }));
    } catch (error) {
      set({ error: getErrorMessage(error) });
      throw error;
    }
  },
}));
