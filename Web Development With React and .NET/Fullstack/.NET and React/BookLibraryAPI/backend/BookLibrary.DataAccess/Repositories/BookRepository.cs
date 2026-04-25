using BookLibrary.Core.Entities;
using BookLibrary.DataAccess.Context;
using Microsoft.EntityFrameworkCore;

namespace BookLibrary.DataAccess.Repositories;

public class BookRepository(LibraryDbContext context)
{
    private readonly LibraryDbContext _context = context;

    public async Task<List<Book>> GetAllAsync()
    {
        return await _context.Books
            .Include(book => book.Tasks)
            .AsNoTracking()
            .OrderByDescending(book => book.Id)
            .ToListAsync();
    }

    public async Task<Book?> GetByIdAsync(int id)
    {
        return await _context.Books
            .Include(book => book.Tasks)
            .AsNoTracking()
            .FirstOrDefaultAsync(book => book.Id == id);
    }

    public async Task<Book?> GetForUpdateAsync(int id)
    {
        return await _context.Books
            .Include(book => book.Tasks)
            .FirstOrDefaultAsync(book => book.Id == id);
    }

    public async Task<bool> ExistsAsync(int id)
    {
        return await _context.Books.AnyAsync(book => book.Id == id);
    }

    public async Task<Book> AddAsync(Book book)
    {
        _context.Books.Add(book);
        await _context.SaveChangesAsync();
        return book;
    }

    public async Task SaveChangesAsync()
    {
        await _context.SaveChangesAsync();
    }

    public async Task DeleteAsync(Book book)
    {
        _context.Books.Remove(book);
        await _context.SaveChangesAsync();
    }
}