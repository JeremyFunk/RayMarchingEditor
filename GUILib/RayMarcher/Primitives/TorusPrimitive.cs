using OpenTK;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GUILib.RayMarcher.Primitives
{
    class TorusPrimitive : Primitive
    {
        float radius;
        float ring_radius;
        public TorusPrimitive(float radius, float ring_radius, Vector3 position, Vector3 rotation, Vector3 scale) : base(position, rotation, scale)
        {
            this.radius = radius;
            this.ring_radius = ring_radius;
        }
        public override ShaderPrimitive GetShaderPrimitive()
        {
            return new ShaderPrimitive { prim_type = 2, values = new float[] { this.radius, this.ring_radius }, transformation = this.transformation, position = this.position };
        }
    }
}
