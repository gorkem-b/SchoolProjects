using BookLibrary.Core.DTOs.Tasks;

namespace BookLibrary.Business.Interfaces;

public interface ITaskService
{
    Task<IReadOnlyCollection<BookTaskDto>> GetAllAsync();
    Task<BookTaskDto?> GetByIdAsync(int id);
    Task<BookTaskDto?> CreateAsync(CreateBookTaskDto createBookTaskDto);
    Task<bool> UpdateAsync(int id, UpdateBookTaskDto updateBookTaskDto);
    Task<bool> DeleteAsync(int id);
}