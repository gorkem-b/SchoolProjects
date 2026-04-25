using BookLibrary.Core.Entities;
using Microsoft.EntityFrameworkCore;

namespace BookLibrary.DataAccess.Context;

public class LibraryDbContext(DbContextOptions<LibraryDbContext> options) : DbContext(options)
{
    public DbSet<Book> Books => Set<Book>();
    public DbSet<BookTask> BookTasks => Set<BookTask>();

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<Book>(entity =>
        {
            entity.Property(book => book.Title)
                .HasMaxLength(200)
                .IsRequired();

            entity.Property(book => book.Author)
                .HasMaxLength(150)
                .IsRequired();

            entity.Property(book => book.Genre)
                .HasMaxLength(100)
                .IsRequired();

            entity.HasMany(book => book.Tasks)
                .WithOne(task => task.Book)
                .HasForeignKey(task => task.BookId)
                .OnDelete(DeleteBehavior.Cascade);
        });

        modelBuilder.Entity<BookTask>(entity =>
        {
            entity.Property(task => task.Title)
                .HasMaxLength(200)
                .IsRequired();

            entity.Property(task => task.Status)
                .HasConversion<string>()
                .HasMaxLength(50)
                .IsRequired();
        });
    }
}