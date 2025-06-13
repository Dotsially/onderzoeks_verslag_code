using System.Diagnostics;
using OpenTK.Graphics.OpenGL4;
using OpenTK.Mathematics;
using OpenTK.Windowing.Common;
using OpenTK.Windowing.Desktop;

class Application : GameWindow
{

    public Application(int width, int height, string title) : base(GameWindowSettings.Default, new NativeWindowSettings() { Size = (width, height), Title = title }) { }

    private int vao;
    private int programID;
    private int indicesLength = 24;

    private Stopwatch stopwatch = new Stopwatch();
    Stopwatch time = new Stopwatch();
    private int count = 0;
    private double min = 1;
    private double max = 0;
    private double[] averages = new double[10];

    public void Initialize()
    {
        time.Start();
        ReadShaders(out programID);
        GL.UseProgram(programID);

        float[] vertices = {
            -1,-1,+1,-1,+1,+1,+1,+1,+1,+1,-1,+1,-1,-1,-1,-1,1,-1,+1,+1,-1,+1,-1,-1
        };

        int[] indices ={
            0, 1, 2, 0, 2, 3, 4, 5, 1, 4, 1, 0, 7, 6, 5, 7, 5, 4, 3, 2, 6, 3, 6, 7
        };

        GL.CreateVertexArrays(1, out vao);
        GL.BindVertexArray(vao);

        int vbo;
        int ebo;
        GL.CreateBuffers(1, out vbo);
        GL.CreateBuffers(1, out ebo);

        GL.BindBuffer(BufferTarget.ArrayBuffer, vbo);
        GL.BufferData(BufferTarget.ArrayBuffer, vertices.Length * sizeof(float), vertices, BufferUsageHint.StaticDraw);

        GL.BindBuffer(BufferTarget.ElementArrayBuffer, ebo);
        GL.BufferData(BufferTarget.ElementArrayBuffer, indices.Length * sizeof(int), indices, BufferUsageHint.StaticDraw);

        GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, 3 * sizeof(float), 0);
        GL.EnableVertexAttribArray(0);

        Matrix4 view = Matrix4.CreateTranslation(new Vector3(0, 0, -3)) * Matrix4.Identity;
        Matrix4 projection = Matrix4.CreatePerspectiveFieldOfView(MathHelper.DegreesToRadians(90.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
        Matrix4 projectionView = view * projection;
        GL.UniformMatrix4(0, false, ref projectionView);

        GL.Enable(EnableCap.DepthTest);
    }

    private void ReadShaders(out int programID)
    {
        String vertex_string = File.ReadAllText("cube_vertex.glsl");
        String fragment_string = File.ReadAllText("cube_fragment.glsl");
        int vertex_shader = GL.CreateShader(ShaderType.VertexShader);
        int fragment_shader = GL.CreateShader(ShaderType.FragmentShader);


        GL.ShaderSource(vertex_shader, vertex_string);
        GL.ShaderSource(fragment_shader, fragment_string);

        GL.CompileShader(vertex_shader);
        GL.CompileShader(fragment_shader);

        programID = GL.CreateProgram();
        GL.AttachShader(programID, vertex_shader);
        GL.AttachShader(programID, fragment_shader);
        GL.LinkProgram(programID);

        GL.DeleteShader(vertex_shader);
        GL.DeleteShader(fragment_shader);
    }

    protected override void OnUpdateFrame(FrameEventArgs e)
    {
        base.OnUpdateFrame(e);

    }

    protected override void OnRenderFrame(FrameEventArgs e)
    {

        stopwatch.Restart();

        base.OnRenderFrame(e);

        GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);
        GL.ClearColor(0.2f, 0.2f, 0.2f, 1.0f);


        Matrix4 transform = Matrix4.CreateRotationY(time.ElapsedMilliseconds * 0.001f);


        GL.UniformMatrix4(1, false, ref transform);
        GL.DrawElements(BeginMode.Triangles, indicesLength, DrawElementsType.UnsignedInt, 0);

        SwapBuffers();
        
        stopwatch.Stop();
        double elapsedMilliseconds = stopwatch.Elapsed.TotalMilliseconds;

        if (elapsedMilliseconds < min){
            min = elapsedMilliseconds;
        }
        if (elapsedMilliseconds > max){
            max = elapsedMilliseconds;
        }

        averages[count] = elapsedMilliseconds;
        count++;
        count %= 10;

        double average = 0;

        foreach (double ms in averages){
            average += ms;
        }
        average /= 10;

        Console.WriteLine($"Render time: {elapsedMilliseconds:F3} ms \n Min: {min:F3} ms \n Average: {average:F3} ms \n Max: {max:F3} ms \n");
    }

}