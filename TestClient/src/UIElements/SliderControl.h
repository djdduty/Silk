#ifndef SILK_SLIDERCONTROL_H
#define SILK_SLIDERCONTROL_H

#include <UI/UIElement.h>
#include <UI/UIText.h>

using namespace Silk;
namespace TestClient
{
    class SliderControl : public UIElement
    {
        public:
            SliderControl();
            ~SliderControl();

            void Update(Scalar dt) { }
            void Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered) { }
        
            void OnMouseMove() {}
            void OnMouseOver() {}
            void OnMouseDown() {}
            void OnMouseOut()  {}
            void OnMouseUp()   {}

        protected:

    };
};

#endif