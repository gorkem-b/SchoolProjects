using BookLibrary.Core.Entities;
using BookLibrary.DataAccess.Context;
using Microsoft.EntityFrameworkCore;

namespace BookLibrary.DataAccess.Repositories;

public class TaskRepository(LibraryDbContext context)
{
    private readonly LibraryDbContext _context = context;

    public async Task<List<BookTask>> GetAllAsync()
    {
        return await _context.BookTasks
            .Include(task => task.Book)
            .AsNoTracking()
            .OrderByDescending(task => task.Id)
            .ToListAsync();
    }

    public async Task<BookTask?> GetByIdAsync(int id)
    {
        return await _context.BookTasks
            .Include(task => task.Book)
            .AsNoTracking()
            .FirstOrDefaultAsync(task => task.Id == id);
    }

    public async Task<BookTask?> GetForUpdateAsync(int id)
    {
        return await _context.BookTasks.FirstOrDefaultAsync(task => task.Id == id);
    }

    public async Task<BookTask> AddAsync(BookTask task)
    {
        _context.BookTasks.Add(task);
        await _context.SaveChangesAsync();
        return task;
    }

    public async Task SaveChangesAsync()
    {
        await _context.SaveChangesAsync();
    }

    public async Task DeleteAsync(BookTask task)
    {
        _context.BookTasks.Remove(task);
        await _context.SaveChangesAsync();
    }
}