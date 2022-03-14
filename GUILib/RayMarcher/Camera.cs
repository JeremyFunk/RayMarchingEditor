using GUILib.Util;
using OpenTK;
using OpenTK.Input;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
namespace GUILib.RayMarcher
{
    class Camera
    {
        private Vector3 position, rotation;
        private float centerRotation = 1;

        public Camera(Vector3 position, Vector3 rotation)
        {
            this.rotation = rotation;
            this.position = position;
        }

        public void UpdateCamera(float delta)
        {
            if (GameInput.IsMouseButtonDown(MouseButton.Middle))
            {
                rotation.Y -= GameInput.mouseDx * 0.2f;
                if (rotation.Y >= 360)
                    rotation.Y = 0;
                if (rotation.Y < 0)
                    rotation.Y = 360;

                rotation.X += GameInput.mouseDy * 0.2f;
                if (rotation.X >= 360)
                    rotation.X = 0;
                if (rotation.X < 0)
                    rotation.X = 360;
            }

            float movementspeed = 2f * delta;
            if (GameInput.isKeyDown(Key.LControl))
                movementspeed = 1f * delta;
            else if (GameInput.isKeyDown(Key.LAlt))
                movementspeed = 4f * delta;

            if (GameInput.isKeyDown(Key.A))
            {
                float yrotrad = (rotation.Y / 180 * 3.141592654f);
                position.X += (float)Math.Cos(yrotrad) * movementspeed;
                position.Z += (float)Math.Sin(yrotrad) * movementspeed;
            }
            else if (GameInput.isKeyDown(Key.D))
            {
                float yrotrad = (rotation.Y / 180 * 3.141592654f);
                position.X -= (float)Math.Cos(yrotrad) * movementspeed;
                position.Z -= (float)Math.Sin(yrotrad) * movementspeed;
            }

            if (GameInput.isKeyDown(Key.W))
            {
                float yrotrad = (rotation.Y / 180 * 3.141592654f);
                float xrotrad = (rotation.X / 180 * 3.141592654f);
                position.X += (float)Math.Sin(yrotrad) * movementspeed;
                position.Z -= (float)Math.Cos(yrotrad) * movementspeed;
                position.Y += (float)Math.Sin(xrotrad) * movementspeed;
            }
            else if (GameInput.isKeyDown(Key.S))
            {
                float yrotrad = (rotation.Y / 180 * 3.141592654f);
                float xrotrad = (rotation.X / 180 * 3.141592654f);
                position.X -= (float)Math.Sin(yrotrad) * movementspeed;
                position.Z += (float)Math.Cos(yrotrad) * movementspeed;
                position.Y -= (float)Math.Sin(xrotrad) * movementspeed;
            }

            if (GameInput.isKeyDown(Key.Space))
            {
                position.Y += movementspeed;
            }
            else if (GameInput.isKeyDown(Key.LShift))
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
