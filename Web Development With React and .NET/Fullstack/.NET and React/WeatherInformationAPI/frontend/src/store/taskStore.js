import { create } from 'zustand';
import api from '../services/api';

const getErrorMessage = (error, fallbackMessage) =>
  error.response?.data?.message ||
  (error.request ? 'Could not reach the API.' : fallbackMessage);

const useTaskStore = create((set) => ({
  tasks: [],
  loading: false,
  error: null,
  fetchTasks: async () => {
    set({ loading: true, error: null });

    try {
      const response = await api.get('/tasks');
      set({ tasks: response.data, loading: false });
    } catch (error) {
      set({ loading: false, error: getErrorMessage(error, 'Could not load tasks.') });
    }
  },
  createTask: async (payload) => {
    set({ error: null });

    try {
      const response = await api.post('/tasks', payload);

      set((state) => ({
        tasks: [response.data, ...state.tasks]
      }));

      return response.data;
    } catch (error) {
      const message = getErrorMessage(error, 'Could not create the task.');
      set({ error: message });
      throw new Error(message);
    }
  },
  updateTaskStatus: async (task, status) => {
    set({ error: null });

    try {
      const response = await api.put(`/tasks/${task.id}`, {
        title: task.title,
        description: task.description,
        category: task.category,
        cityName: task.cityName,
        status
      });

      set((state) => ({
        tasks: state.tasks.map((item) => (item.id === task.id ? response.data : item))
      }));

      return response.data;
    } catch (error) {
      const message = getErrorMessage(error, 'Could not update the task.');
      set({ error: message });
      throw new Error(message);
    }
  },
  deleteTask: async (taskId) => {
    set({ error: null });

    try {
      await api.delete(`/tasks/${taskId}`);

      set((state) => ({
        tasks: state.tasks.filter((item) => item.id !== taskId)
      }));
    } catch (error) {
      const message = getErrorMessage(error, 'Could not delete the task.');
      set({ error: message });
      throw new Error(message);
    }
  }
}));

export default useTaskStore;
