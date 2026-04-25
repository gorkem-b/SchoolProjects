using BookLibrary.Core.DTOs.Tasks;

namespace BookLibrary.Core.DTOs.Books;

public class BookDto
{
    public int Id { get; set; }
    public string Title { get; set; } = string.Empty;
    public string Author { get; set; } = string.Empty;
    public string Genre { get; set; } = string.Empty;
    public IReadOnlyCollection<BookTaskDto> Tasks { get; set; } = [];
}