using GUILib.Util;
using OpenTK;
using OpenTK.Input;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
namespace RayMarcher.RayMarcher
{
    class Camera
    {
        private Vector3 position, rotation, focus;

        public Camera(Vector3 position, Vector3 rotation)
        {
            this.rotation = rotation;
            this.position = position;
            this.focus = new Vector3(0);
        }

        public void UpdateCamera(float delta)
        {
            if (GameInput.IsMouseButtonDown(MouseButton.Middle))
            {
                rotation.Y -= GameInput.mouseDx * 0.4f;
                if (rotation.Y >= 360)
                    rotation.Y = 0;
                if (rotation.Y < 0)
                    rotation.Y = 360;

                rotation.X += GameInput.mouseDy * 0.4f;
                if (rotation.X >= 360)
                    rotation.X = 0;
                if (rotation.X < 0)
                    rotation.X = 360;
            }

            float movementspeed = 8f * delta;
            if (GameInput.IsKeyDown(Key.LControl))
                movementspeed = 16f * delta;
            else if (GameInput.IsKeyDown(Key.LAlt))
                movementspeed = 1f * delta;

            if(GameInput.mouseWheel < 0)
            {
                var diff = position - focus;
                position += diff * .1f;
            }
            else if (GameInput.mouseWheel > 0)
            {
                var diff = position - focus;
                position -= diff * .1f;
            }

            if (GameInput.IsKeyDown(Key.A))
            {
                float yrotrad = (rotation.Y / 180 * 3.141592654f);
                position.X += (float)Math.Cos(-yrotrad) * movementspeed;
                position.Z += (float)Math.Sin(-yrotrad) * movementspeed;
            }
            else if (GameInput.IsKeyDown(Key.D))
            {
                float yrotrad = (rotation.Y / 180 * 3.141592654f);
                position.X -= (float)Math.Cos(-yrotrad) * movementspeed;
                position.Z -= (float)Math.Sin(-yrotrad) * movementspeed;
            }

            if (GameInput.IsKeyDown(Key.W))
            {
                float yrotrad = (rotation.Y / 180 * 3.141592654f);
                float xrotrad = (rotation.X / 180 * 3.141592654f);
                position.X += (float)Math.Sin(-yrotrad) * movementspeed;
                position.Z -= (float)Math.Cos(-yrotrad) * movementspeed;
                //position.Y += (float)Math.Sin(xrotrad) * movementspeed;
            }
            else if (GameInput.IsKeyDown(Key.S))
            {
                float yrotrad = (rotation.Y / 180 * 3.141592654f);
                float xrotrad = (rotation.X / 180 * 3.141592654f);
                position.X -= (float)Math.Sin(-yrotrad) * movementspeed;
                position.Z += (float)Math.Cos(-yrotrad) * movementspeed;
                //position.Y -= (float)Math.Sin(xrotrad) * movementspeed;
            }

            if (GameInput.IsKeyDown(Key.Space))
            {
                position.Y += movementspeed;
            }
            else if (GameInput.IsKeyDown(Key.LShift))
            {
                position.Y -= movementspeed;
            }
        
        }

        public (Vector3, Vector3) GetPosition()
        {
            return (position, rotation);
        }
    }
}
