using BookLibrary.Core.Enums;

namespace BookLibrary.Core.Entities;

public class BookTask
{
    public int Id { get; set; }
    public string Title { get; set; } = string.Empty;
    public BookTaskStatus Status { get; set; } = BookTaskStatus.ToRead;
    public int BookId { get; set; }
    public Book? Book { get; set; }
}