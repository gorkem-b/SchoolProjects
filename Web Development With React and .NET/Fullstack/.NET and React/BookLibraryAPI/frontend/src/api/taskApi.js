import httpClient from './http'

export async function getTasks() {
  const response = await httpClient.get('/tasks')
  return response.data
}

export async function createTask(payload) {
  const response = await httpClient.post('/tasks', payload)
  return response.data
}

export async function updateTask(id, payload) {
  await httpClient.put(`/tasks/${id}`, payload)
}

export async function deleteTask(id) {
  await httpClient.delete(`/tasks/${id}`)
}