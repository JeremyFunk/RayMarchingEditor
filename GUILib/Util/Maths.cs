using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OpenTK;
using GUILib.GUI.GuiElements;

namespace GUILib.Util
{
    static class Utility
    {
        public static List<GuiElement> GetZIndexSorted(List<GuiElement> elements)
        {
            elements.Sort((e1, e2) => e1.ZIndex.CompareTo(e2.ZIndex));

            return elements;
        }
        public static float DegreeToRadian(float angle)
        {
            return (float)(Math.PI * angle / 180.0f);
        }

        public static double RadianToDegree(float angle)
        {
            return angle * (180.0f / Math.PI);
        }
    }
    static class MathsGeometry
    {
        public static bool IsInsideQuad(Vector2 p, float smallestX, float smallestY, float biggestX, float biggestY)
        {
            if (p.X < smallestX || p.X > biggestX || p.Y < smallestY || p.Y > biggestY)
                return false;
            return true;
        }

        public static bool IsInsideQuad(Vector2 p, GuiElement element)
        {
            if (!element.IsAnimationRunning()) { 
                if (p.X < element.curX + element.animationOffsetX || p.X > element.curX + element.curWidth + element.animationOffsetX + element.animationOffsetWidth || p.Y < element.curY + element.animationOffsetY || p.Y > element.curY + element.curHeight + element.animationOffsetY + element.animationOffsetHeight)
                    return false;
            }else
                if (p.X < element.curX || p.X > element.curX + element.curWidth || p.Y < element.curY || p.Y > element.curY + element.curHeight)
                    return false;
            return true;
        }
    }
    static class MathsMatrix
    {
        public static object EngineConstants { get; private set; }

        public static Matrix3 CreateTransformationMatrix3(Vector3 rotation, Vector3 scale)
        {
            Matrix3 matrix = Matrix3.Identity;
            matrix *= Matrix3.CreateRotationX((float)Utility.DegreeToRadian(rotation.X));
            matrix *= Matrix3.CreateRotationY((float)Utility.DegreeToRadian(rotation.Y));
            matrix *= Matrix3.CreateRotationZ((float)Utility.DegreeToRadian(rotation.Z));
            matrix *= Matrix3.CreateScale(scale);
            
            return matrix;
        }

        public static Matrix3 CreateRotationMatrix3(Vector3 rotation)
        {
            Matrix3 matrix = Matrix3.Identity;
            matrix *= Matrix3.CreateRotationX((float)Utility.DegreeToRadian(rotation.X));
            matrix *= Matrix3.CreateRotationY((float)Utility.DegreeToRadian(rotation.Y));
            matrix *= Matrix3.CreateRotationZ((float)Utility.DegreeToRadian(rotation.Z));

            return matrix;
        }
        public static Matrix4 CreateTransformationMatrix(Vector3 translation, float rx, float ry, float rz, float scale)
        {
            Matrix4 matrix = Matrix4.Identity;
            matrix *= Matrix4.CreateRotationX((float)Utility.DegreeToRadian(rx));
            matrix *= Matrix4.CreateRotationY((float)Utility.DegreeToRadian(ry));
            matrix *= Matrix4.CreateRotationZ((float)Utility.DegreeToRadian(rz));
            matrix *= Matrix4.CreateScale(scale);
            matrix *= Matrix4.CreateTranslation(translation);

            return matrix;
        }

        public static Matrix4 CreateTransformationMatrix(Vector3 translation, float rx, float ry, float rz, Vector3 scale)
        {
            Matrix4 matrix = Matrix4.Identity;
            matrix *= Matrix4.CreateRotationX((float)Utility.DegreeToRadian(rx));
            matrix *= Matrix4.CreateRotationY((float)Utility.DegreeToRadian(ry));
            matrix *= Matrix4.CreateRotationZ((float)Utility.DegreeToRadian(rz));
            matrix *= Matrix4.CreateScale(scale);
            matrix *= Matrix4.CreateTranslation(translation);
            return matrix;
        }

        public static Matrix4 CreateTransformationMatrix(Vector3 translation, Vector3 rotation, Vector3 scale)
        {
            Matrix4 matrix = Matrix4.Identity;
            matrix *= Matrix4.CreateRotationX((float)Utility.DegreeToRadian(rotation.X));
            matrix *= Matrix4.CreateRotationY((float)Utility.DegreeToRadian(rotation.Y));
            matrix *= Matrix4.CreateRotationZ((float)Utility.DegreeToRadian(rotation.Z));
            matrix *= Matrix4.CreateScale(scale);
            matrix *= Matrix4.CreateTranslation(translation);

            return matrix;
        }

        public static Matrix4 CreateTransformationMatrix(float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz)
        {
            Matrix4 matrix = Matrix4.Identity;
            matrix *= Matrix4.CreateRotationX((float)Utility.DegreeToRadian(rx));
            matrix *= Matrix4.CreateRotationY((float)Utility.DegreeToRadian(ry));
            matrix *= Matrix4.CreateRotationZ((float)Utility.DegreeToRadian(rz));
            matrix *= Matrix4.CreateScale(sx, sy, sz);
            matrix *= Matrix4.CreateTranslation(px, py, pz);

            return matrix;
        }

        public static Matrix4 CreateTransformationMatrix(float px, float py, float pz, float rx, float ry, float rz, float s)
        {
            Matrix4 matrix = Matrix4.Identity;
            matrix *= Matrix4.CreateRotationX((float)Utility.DegreeToRadian(rx));
            matrix *= Matrix4.CreateRotationY((float)Utility.DegreeToRadian(ry));
            matrix *= Matrix4.CreateRotationZ((float)Utility.DegreeToRadian(rz));
            matrix *= Matrix4.CreateScale(s);
            matrix *= Matrix4.CreateTranslation(px, py, pz);

            return matrix;
        }


        public static Matrix4 CreateViewMatrix(Vector3 cameraPos, float pitch, float yaw, float roll)
        {
            Matrix4 viewMatrix = Matrix4.Identity;

            Vector3 negativeCameraPos = new Vector3(-cameraPos.X, -cameraPos.Y, -cameraPos.Z);
            viewMatrix *= Matrix4.CreateTranslation(negativeCameraPos);

            Matrix4 orientation = Matrix4.Identity;
            orientation *= Matrix4.CreateRotationY((float)Utility.DegreeToRadian(yaw));
            orientation *= Matrix4.CreateRotationX((float)Utility.DegreeToRadian(-pitch));

            viewMatrix *= orientation;

            return viewMatrix;
        }

        public static Vector4 TransformMatrix(Matrix4 left, Vector4 right)
        {
            Vector4 dest = new Vector4();

            float x = left.Column0.X * right.X + left.Column1.X * right.Y + left.Column2.X * right.Z + left.Column3.X * right.W;
            float y = left.Column0.Y * right.X + left.Column1.Y * right.Y + left.Column2.Y * right.Z + left.Column3.Y * right.W;
            float z = left.Column0.Z * right.X + left.Column1.Z * right.Y + left.Column2.Z * right.Z + left.Column3.Z * right.W;
            float w = left.Column0.W * right.X + left.Column1.W * right.Y + left.Column2.W * right.Z + left.Column3.W * right.W;

            dest.X = x;
            dest.Y = y;
            dest.Z = z;
            dest.W = w;

            return dest;
        }

        public static Matrix4 FaceCameraViewModelMatrix(Matrix4 viewMatrix, Matrix4 modelMatrix)
        {
            modelMatrix.M11 = viewMatrix.M11;
            modelMatrix.M12 = viewMatrix.M21;
            modelMatrix.M13 = viewMatrix.M31;

            modelMatrix.M21 = viewMatrix.M12;
            modelMatrix.M22 = viewMatrix.M22;
            modelMatrix.M23 = viewMatrix.M32;

            modelMatrix.M31 = viewMatrix.M13;
            modelMatrix.M32 = viewMatrix.M23;
            modelMatrix.M33 = viewMatrix.M33;

            return viewMatrix * modelMatrix;
        }
    }
}
