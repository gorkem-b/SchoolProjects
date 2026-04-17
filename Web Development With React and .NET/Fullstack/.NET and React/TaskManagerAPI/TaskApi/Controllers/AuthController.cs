using Microsoft.AspNetCore.Mvc;
using TaskApi.DTOs;
using TaskApi.Services;

namespace TaskApi.Controllers;

[ApiController]
[Route("api/auth")]
public class AuthController : ControllerBase
{
    private readonly AuthService _authService;

    public AuthController(AuthService authService)
    {
        _authService = authService;
    }

    [HttpPost("register")]
    public async Task<IActionResult> Register([FromBody] UserAuthDto dto)
    {
        if (string.IsNullOrWhiteSpace(dto.Username) || string.IsNullOrWhiteSpace(dto.Password))
        {
            return BadRequest(new { message = "Username and password are required." });
        }

        var isRegistered = await _authService.RegisterAsync(dto);
        if (!isRegistered)
        {
            return Conflict(new { message = "This username is already in use." });
        }

        return Ok(new
        {
            message = "User created successfully."
        });
    }

    [HttpPost("login")]
    public async Task<IActionResult> Login([FromBody] UserAuthDto dto)
    {
        if (string.IsNullOrWhiteSpace(dto.Username) || string.IsNullOrWhiteSpace(dto.Password))
        {
            return BadRequest(new { message = "Username and password are required." });
        }

        var token = await _authService.LoginAsync(dto);
        if (token is null)
        {
            return Unauthorized(new { message = "Invalid username or password." });
        }

        return Ok(new
        {
            token
        });
    }
}