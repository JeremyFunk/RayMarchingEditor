using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OpenTK.Graphics.OpenGL;
using GUILib.Util;
using OpenTK;
using GUILib.RayMarcher.Primitives;

namespace GUILib.GUI.Render.Shader
{
    class FractalShader : ShaderProgram
    {
        private static readonly string
            resolutionUniform = "u_resolution",

            primitiveUniform = "u_primitives";

        private int quadVao;

        public FractalShader(int quadVao) : base(@"Shaders\fractal.vs", @"Shaders\fractal.fs") { this.quadVao = quadVao; }

        public override void LoadUniforms(){}
        

        private int currentVaoID = 0;
        public void SetRenderVAO(int vaoID)
        {
            if (vaoID != currentVaoID)
            {
                currentVaoID = vaoID;
                GL.BindVertexArray(vaoID);
                GL.EnableVertexAttribArray(0);
                GL.EnableVertexAttribArray(1);
            }
        }

        public void ResetVAO()
        {
            if (quadVao != currentVaoID)
            {
                currentVaoID = quadVao;
                GL.BindVertexArray(quadVao);
                GL.EnableVertexAttribArray(0);
                GL.EnableVertexAttribArray(1);
            }
        }

        public void UpdateResolution()
        {
            Start();
            SetUniform(resolutionUniform, new Vector2(GameSettings.Width, GameSettings.Height));
            Stop();
        }

        public void UpdatePrimitivies(List<Primitive> primitives)
        {
            Start();
            for(int i = 0; i < primitives.Count; i++)
            {
                var shaderPrim = primitives[0].GetShaderPrimitive();
                SetUniform($"{primitiveUniform}[{i}].prim_type", shaderPrim.prim_type);
                SetUniform($"{primitiveUniform}[{i}].transformation", shaderPrim.transformation);
                SetUniform($"{primitiveUniform}[{i}].position", shaderPrim.position);
                for (int j = 0; j < shaderPrim.values.Length; j++)
                {
                    SetUniform($"{primitiveUniform}[{i}].attribute{j}", shaderPrim.values[j]);
                }
            }
            Stop();
        }

        public void UpdateCamera(Vector3 pos, Matrix3 rot)
        {
            Start();
            SetUniform("camera_rot", rot);
            SetUniform("camera_pos", pos);
            Stop();
        }

        private Vector2 WindowScale(Vector2 scale)
        {
            return new Vector2(scale.X / GameSettings.Width, scale.Y / GameSettings.Height);
        }
    }
}
