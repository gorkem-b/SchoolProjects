import httpClient from './http'

export async function getBooks() {
  const response = await httpClient.get('/books')
  return response.data
}

export async function createBook(payload) {
  const response = await httpClient.post('/books', payload)
  return response.data
}

export async function deleteBook(id) {
  await httpClient.delete(`/books/${id}`)
}