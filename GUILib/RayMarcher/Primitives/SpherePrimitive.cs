using OpenTK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GUILib.RayMarcher.Primitives
{
    class SpherePrimitive : Primitive
    {
        float radius;
        public SpherePrimitive(float radius, Vector3 position, Vector3 rotation, Vector3 scale) : base(position, rotation, scale)
        {
            this.radius = radius;
        }
        public override ShaderPrimitive GetShaderPrimitive()
        {
            return new ShaderPrimitive { prim_type = 1, values = new float[] { this.radius }, transformation = this.transformation, position = this.position };
        }
    }
}
