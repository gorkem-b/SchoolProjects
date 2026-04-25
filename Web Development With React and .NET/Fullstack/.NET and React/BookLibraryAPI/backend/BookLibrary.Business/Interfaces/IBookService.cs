using BookLibrary.Core.DTOs.Books;

namespace BookLibrary.Business.Interfaces;

public interface IBookService
{
    Task<IReadOnlyCollection<BookDto>> GetAllAsync();
    Task<BookDto?> GetByIdAsync(int id);
    Task<BookDto> CreateAsync(CreateBookDto createBookDto);
    Task<bool> UpdateAsync(int id, UpdateBookDto updateBookDto);
    Task<bool> DeleteAsync(int id);
}