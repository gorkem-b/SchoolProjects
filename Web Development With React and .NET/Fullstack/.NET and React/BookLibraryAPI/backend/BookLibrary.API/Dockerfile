FROM mcr.microsoft.com/dotnet/sdk:10.0 AS build
WORKDIR /src

COPY BookLibrary.slnx ./
COPY BookLibrary.API/BookLibrary.API.csproj BookLibrary.API/
COPY BookLibrary.Business/BookLibrary.Business.csproj BookLibrary.Business/
COPY BookLibrary.Core/BookLibrary.Core.csproj BookLibrary.Core/
COPY BookLibrary.DataAccess/BookLibrary.DataAccess.csproj BookLibrary.DataAccess/

RUN dotnet restore BookLibrary.API/BookLibrary.API.csproj

COPY . .
WORKDIR /src/BookLibrary.API
RUN dotnet publish BookLibrary.API.csproj -c Release -o /app/publish /p:UseAppHost=false

FROM mcr.microsoft.com/dotnet/aspnet:10.0 AS final
WORKDIR /app

ENV ASPNETCORE_URLS=http://+:8080
EXPOSE 8080

COPY --from=build /app/publish .

ENTRYPOINT ["dotnet", "BookLibrary.API.dll"]