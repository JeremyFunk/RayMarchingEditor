using OpenTK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RayMarcher.RayMarcher.Primitives
{
    class CubePrimitive : Primitive
    {
        float x;
        float y;
        float z;
        public CubePrimitive(float x, float y, float z, Vector3 position, Vector3 rotation, Vector3 scale) : base(position, rotation, scale)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }
        public override ShaderPrimitive GetShaderPrimitive()
        {
            return new ShaderPrimitive { prim_type = 3, values = new float[] { this.x, this.y, this.z }, transformation = this.transformation, position = this.position };
        }
    }
}
