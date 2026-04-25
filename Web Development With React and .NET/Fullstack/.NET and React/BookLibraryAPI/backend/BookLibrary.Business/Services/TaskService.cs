using BookLibrary.Business.Interfaces;
using BookLibrary.Business.Mappings;
using BookLibrary.Core.DTOs.Tasks;
using BookLibrary.Core.Entities;
using BookLibrary.DataAccess.Repositories;

namespace BookLibrary.Business.Services;

public class TaskService(TaskRepository taskRepository, BookRepository bookRepository) : ITaskService
{
    private readonly TaskRepository _taskRepository = taskRepository;
    private readonly BookRepository _bookRepository = bookRepository;

    public async Task<IReadOnlyCollection<BookTaskDto>> GetAllAsync()
    {
        var tasks = await _taskRepository.GetAllAsync();
        return tasks.Select(task => task.ToDto()).ToArray();
    }

    public async Task<BookTaskDto?> GetByIdAsync(int id)
    {
        var task = await _taskRepository.GetByIdAsync(id);
        return task?.ToDto();
    }

    public async Task<BookTaskDto?> CreateAsync(CreateBookTaskDto createBookTaskDto)
    {
        var bookExists = await _bookRepository.ExistsAsync(createBookTaskDto.BookId);
        if (!bookExists)
        {
            return null;
        }

        var task = new BookTask
        {
            Title = createBookTaskDto.Title.Trim(),
            Status = createBookTaskDto.Status,
            BookId = createBookTaskDto.BookId
        };

        await _taskRepository.AddAsync(task);

        var createdTask = await _taskRepository.GetByIdAsync(task.Id);
        return createdTask?.ToDto();
    }

    public async Task<bool> UpdateAsync(int id, UpdateBookTaskDto updateBookTaskDto)
    {
        var task = await _taskRepository.GetForUpdateAsync(id);
        if (task is null)
        {
            return false;
        }

        task.Title = updateBookTaskDto.Title.Trim();
        task.Status = updateBookTaskDto.Status;

        await _taskRepository.SaveChangesAsync();
        return true;
    }

    public async Task<bool> DeleteAsync(int id)
    {
        var task = await _taskRepository.GetForUpdateAsync(id);
        if (task is null)
        {
            return false;
        }

        await _taskRepository.DeleteAsync(task);
        return true;
    }
}