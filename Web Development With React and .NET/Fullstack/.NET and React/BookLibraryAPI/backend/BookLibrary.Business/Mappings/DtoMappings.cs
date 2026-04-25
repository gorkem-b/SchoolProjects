using BookLibrary.Core.DTOs.Books;
using BookLibrary.Core.DTOs.Tasks;
using BookLibrary.Core.Entities;

namespace BookLibrary.Business.Mappings;

public static class DtoMappings
{
    public static BookDto ToDto(this Book book)
    {
        return new BookDto
        {
            Id = book.Id,
            Title = book.Title,
            Author = book.Author,
            Genre = book.Genre,
            Tasks = book.Tasks
                .OrderBy(task => task.Id)
                .Select(task => task.ToDto())
                .ToArray()
        };
    }

    public static BookTaskDto ToDto(this BookTask task)
    {
        return new BookTaskDto
        {
            Id = task.Id,
            Title = task.Title,
            Status = task.Status,
            BookId = task.BookId,
            BookTitle = task.Book?.Title ?? string.Empty
        };
    }
}