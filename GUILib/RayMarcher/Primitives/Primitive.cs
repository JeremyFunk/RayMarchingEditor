using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GUILib.Util;
using OpenTK;

namespace GUILib.RayMarcher.Primitives
{
    struct ShaderPrimitive
    {
        public float[] values;
        public int prim_type;
        public Matrix3 transformation;
        public Vector3 position;
    }

    abstract class Primitive
    {
        protected Vector3 position;
        protected Vector3 rotation;
        protected Vector3 scale;
        public Matrix3 transformation;
        public Primitive(Vector3 position, Vector3 rotation, Vector3 scale)
        {
            this.position = position;
            this.rotation = rotation;
            this.scale = scale;
            this.transformation = MathsMatrix.CreateTransformationMatrix3(rotation, scale);
        }

        public abstract ShaderPrimitive GetShaderPrimitive();
    }
}
