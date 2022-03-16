using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OpenTK;
using OpenTK.Graphics.OpenGL;
using GUILib.Util;
using GUILib.GUI.GuiElements;
using GUILib.Logger;

namespace RayMarcher.RayMarcher.Renderer
{
    class FractalRenderer
    {
        private FractalShader shader;
        private int vaoID;

        public FractalRenderer()
        {
            float[] vertices = new float[]{
                 -1,  1, 0,
                  1,  1, 0,
                  1, -1, 0,
                 -1, -1, 0
            };

            float[] textureCoords = new float[]
            {
                0, 0,
                1, 0,
                1, 1,
                0, 1
            };

            ALogger.defaultLogger.Log("Loading quad vertices.", LogLevel.Info);
            vaoID = OpenGLLoader.LoadObject(vertices, textureCoords);

            shader = new FractalShader(vaoID);
        }

        public void Render()
        {
            shader.Start();

            GL.BindVertexArray(vaoID);
            GL.EnableVertexAttribArray(0);
            GL.EnableVertexAttribArray(1);

            GL.DrawArrays(PrimitiveType.Quads, 0, 4);

            GL.DisableVertexAttribArray(1);
            GL.DisableVertexAttribArray(0);
            GL.BindVertexArray(0);

            shader.Stop();
        }

        public FractalShader GetShader()
        {
            return shader;
        }

        internal void CleanUp()
        {
            shader.CleanUp();
        }

        internal void PrepareRender()
        {
            GL.Disable(EnableCap.ScissorTest);
            GL.Disable(EnableCap.StencilTest);
            GL.Enable(EnableCap.DepthTest);
            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.StencilBufferBit | ClearBufferMask.DepthBufferBit);
        }
    }
}
