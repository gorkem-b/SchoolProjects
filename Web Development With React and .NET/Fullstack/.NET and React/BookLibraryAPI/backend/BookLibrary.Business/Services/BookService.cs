using BookLibrary.Business.Interfaces;
using BookLibrary.Business.Mappings;
using BookLibrary.Core.DTOs.Books;
using BookLibrary.Core.Entities;
using BookLibrary.DataAccess.Repositories;

namespace BookLibrary.Business.Services;

public class BookService(BookRepository bookRepository) : IBookService
{
    private readonly BookRepository _bookRepository = bookRepository;

    public async Task<IReadOnlyCollection<BookDto>> GetAllAsync()
    {
        var books = await _bookRepository.GetAllAsync();
        return books.Select(book => book.ToDto()).ToArray();
    }

    public async Task<BookDto?> GetByIdAsync(int id)
    {
        var book = await _bookRepository.GetByIdAsync(id);
        return book?.ToDto();
    }

    public async Task<BookDto> CreateAsync(CreateBookDto createBookDto)
    {
        var book = new Book
        {
            Title = createBookDto.Title.Trim(),
            Author = createBookDto.Author.Trim(),
            Genre = createBookDto.Genre.Trim()
        };

        await _bookRepository.AddAsync(book);

        var createdBook = await _bookRepository.GetByIdAsync(book.Id);
        return createdBook!.ToDto();
    }

    public async Task<bool> UpdateAsync(int id, UpdateBookDto updateBookDto)
    {
        var book = await _bookRepository.GetForUpdateAsync(id);
        if (book is null)
        {
            return false;
        }

        book.Title = updateBookDto.Title.Trim();
        book.Author = updateBookDto.Author.Trim();
        book.Genre = updateBookDto.Genre.Trim();

        await _bookRepository.SaveChangesAsync();
        return true;
    }

    public async Task<bool> DeleteAsync(int id)
    {
        var book = await _bookRepository.GetForUpdateAsync(id);
        if (book is null)
        {
            return false;
        }

        await _bookRepository.DeleteAsync(book);
        return true;
    }
}