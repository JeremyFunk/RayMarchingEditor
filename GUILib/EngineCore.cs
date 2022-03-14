using OpenTK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GUILib.Util;
using GUILib.GUI.Render;
using GUILib.GUI.GuiElements;
using GUILib.GUI;
using GUILib.GUI.Constraints;
using GUILib.GUI.Animations;
using GUILib.Events;

using OpenTK.Graphics.OpenGL;
using OpenTK.Input;
using GUILib.GUI.Animations.Transitions;
using GUILib.GUI.PixelConstraints;
using GUILib.RayMarcher.Primitives;
using GUILib.RayMarcher;

namespace GUILib
{
    class EngineCore : GameWindow
    {
        private GuiRenderer guiRenderer;
        private GuiScene scene;
        private List<Primitive> primitives;
        private FractalRenderer fractalRenderer;
        private Camera camera;


        public EngineCore(int widthP, int heightP, string title) : base(widthP, heightP, new OpenTK.Graphics.GraphicsMode(new OpenTK.Graphics.ColorFormat(8, 8, 8, 8), 24, 8, 4))
        {
            
        }

        protected override void OnLoad(EventArgs e)
        {
            GameSettings.Width = Width;
            GameSettings.Height = Height;

            //guiRenderer = new GuiRenderer();
            //scene = new GuiScene();

            //FirstMenu m = new FirstMenu(scene);
            //ModernMenu m = new ModernMenu(scene);

            GameInput.Initialize();

            GL.Enable(EnableCap.AlphaTest);
            GL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);
            GL.Enable(EnableCap.Blend);
            GL.Enable(EnableCap.DepthTest);

            fractalRenderer = new FractalRenderer();
            fractalRenderer.GetShader().UpdateResolution();
            primitives = new List<Primitive>();
            //primitives.Add(new SpherePrimitive(1.0f, new Vector3(0), new Vector3(0), new Vector3(1)));
            //primitives.Add(new TorusPrimitive(1.0f, 0.5f));
            primitives.Add(new CubePrimitive(1.0f, 1.0f, 1.0f, new Vector3(0, 0, -5), new Vector3(0, 45, 0), new Vector3(1)));
            //primitives.Add(new MandelbulbPrimitive(3.0f, new Vector3(0), new Vector3(0), new Vector3(1)));
            fractalRenderer.GetShader().UpdatePrimitivies(primitives);
            camera = new Camera(new Vector3(0, 0, 3), new Vector3(0));
        }
        
        
        protected override void OnResize(EventArgs e)
        {
            GameSettings.Width = Width;
            GameSettings.Height = Height;

            GL.Viewport(0, 0, Width, Height);
            fractalRenderer.GetShader().UpdateResolution();

            base.OnResize(e);
        }


        protected override void OnUpdateFrame(FrameEventArgs e)
        {
            camera.UpdateCamera((float)e.Time);
            //scene.Update((float)e.Time);
            fractalRenderer.GetShader().UpdatePrimitivies(primitives);
            var (position, rotation) = camera.GetPosition();
            Matrix3 mat = MathsMatrix.CreateRotationMatrix3(rotation);
            fractalRenderer.GetShader().UpdateCamera(position, mat);
            GameInput.Update();
        }
        protected override void OnRenderFrame(FrameEventArgs e)
        {
            base.OnRenderFrame(e);
            fractalRenderer.PrepareRender();
            fractalRenderer.Render();
            this.SwapBuffers();
        }






        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);

            //guiRenderer.CleanUp();
        }










        protected override void OnKeyUp(KeyboardKeyEventArgs e)
        {
            base.OnKeyUp(e);
            GameInput.UpdateKey(e.Key, false);
        }

        protected override void OnKeyDown(KeyboardKeyEventArgs e)
        {
            base.OnKeyDown(e);

            GameInput.UpdateKey(e.Key, true);
        }

        protected override void OnMouseMove(MouseMoveEventArgs e)
        {
            base.OnMouseMove(e);

            GameInput.mouseDx = GameInput.mouseX - e.X;
            GameInput.mouseDy = GameInput.mouseY - e.Y;

            GameInput.mouseX = e.X;
            GameInput.mouseY = e.Y;

            GameInput.normalizedMouseX = (2f * e.X) / Width - 1;
            GameInput.normalizedMouseY = -1 * ((2f * e.Y) / Height - 1);
        }

        protected override void OnMouseWheel(MouseWheelEventArgs e)
        {
            base.OnMouseWheel(e);

            GameInput.mouseWheel = e.Delta;
            GameInput.mouseWheelF = e.DeltaPrecise;
        }

        protected override void OnMouseLeave(EventArgs e)
        {
            base.OnMouseLeave(e);
            GameInput.mouseInside = false;
        }

        protected override void OnMouseEnter(EventArgs e)
        {
            base.OnMouseLeave(e);
            GameInput.mouseInside = true;
        }

        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            base.OnMouseDown(e);
            GameInput.UpdateMouseButton(e.Button, true);
        }

        protected override void OnMouseUp(MouseButtonEventArgs e)
        {
            base.OnMouseUp(e);
            GameInput.UpdateMouseButton(e.Button, false);
        }
    }
}
