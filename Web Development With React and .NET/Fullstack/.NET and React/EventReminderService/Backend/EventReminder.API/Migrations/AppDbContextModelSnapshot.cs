using System;
using EventReminder.API.Data;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Infrastructure;
using Microsoft.EntityFrameworkCore.Metadata;

#nullable disable

namespace EventReminder.API.Migrations;

[DbContext(typeof(AppDbContext))]
partial class AppDbContextModelSnapshot : ModelSnapshot
{
    protected override void BuildModel(ModelBuilder modelBuilder)
    {
#pragma warning disable 612, 618
        modelBuilder
            .HasAnnotation("ProductVersion", "10.0.0")
            .HasAnnotation("Relational:MaxIdentifierLength", 128);

        SqlServerModelBuilderExtensions.UseIdentityColumns(modelBuilder);

        modelBuilder.Entity("EventReminder.API.Models.TaskItem", b =>
        {
            b.Property<int>("Id")
                .ValueGeneratedOnAdd()
                .HasColumnType("int");

            SqlServerPropertyBuilderExtensions.UseIdentityColumn(b.Property<int>("Id"));

            b.Property<string>("Category")
                .IsRequired()
                .HasMaxLength(80)
                .HasColumnType("nvarchar(80)");

            b.Property<DateTime>("CreatedAt")
                .ValueGeneratedOnAdd()
                .HasColumnType("datetime2")
                .HasDefaultValueSql("SYSUTCDATETIME()");

            b.Property<string>("Description")
                .HasMaxLength(1000)
                .HasColumnType("nvarchar(1000)");

            b.Property<DateTime>("DueDate")
                .HasColumnType("datetime2");

            b.Property<bool>("IsNotified")
                .ValueGeneratedOnAdd()
                .HasColumnType("bit")
                .HasDefaultValue(false);

            b.Property<string>("Status")
                .IsRequired()
                .ValueGeneratedOnAdd()
                .HasMaxLength(30)
                .HasColumnType("nvarchar(30)")
                .HasDefaultValue("Pending");

            b.Property<string>("Title")
                .IsRequired()
                .HasMaxLength(120)
                .HasColumnType("nvarchar(120)");

            b.Property<DateTime?>("UpdatedAt")
                .HasColumnType("datetime2");

            b.HasKey("Id");

            b.HasIndex("DueDate");

            b.HasIndex("Status", "IsNotified");

            b.ToTable("TaskItems");
        });
#pragma warning restore 612, 618
    }
}
