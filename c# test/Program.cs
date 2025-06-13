

class Program
{
    static void Main()
    {
        using (Application application = new Application(1280, 720, "Cube Test"))
        {
            application.Initialize();
            application.Run();
        }
    }
}
