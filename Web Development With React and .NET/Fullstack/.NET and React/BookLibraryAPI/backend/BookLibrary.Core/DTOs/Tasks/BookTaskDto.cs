using BookLibrary.Core.Enums;

namespace BookLibrary.Core.DTOs.Tasks;

public class BookTaskDto
{
    public int Id { get; set; }
    public string Title { get; set; } = string.Empty;
    public BookTaskStatus Status { get; set; }
    public int BookId { get; set; }
    public string BookTitle { get; set; } = string.Empty;
}