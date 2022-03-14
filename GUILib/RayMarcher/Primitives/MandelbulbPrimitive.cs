using OpenTK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GUILib.RayMarcher.Primitives
{
    class MandelbulbPrimitive : Primitive
    {
        float power;
        public MandelbulbPrimitive(float power, Vector3 position, Vector3 rotation, Vector3 scale): base(position, rotation, scale)
        {
            this.power = power;
        }
        public override ShaderPrimitive GetShaderPrimitive()
        {
            return new ShaderPrimitive { prim_type = 4, values = new float[] { this.power }, transformation = this.transformation, position = this.position };
        }
    }
}
