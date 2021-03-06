#pragma once
#include "../dsp/utilities.h"
#include "vstgui/vstgui.h"
#include "vstgui/vstgui_uidescription.h" // for IController

const float KNOB_FACTOR_DEFAULT = 1.0f;
const float KNOB_FACTOR_GOES_TO_ELEVEN = 11.0f;

namespace VSTGUI
{
    /**
    \class KickSwitchController
    \ingroup Custom-SubControllers
    \brief
    -	use the sub-controller string "KickSwitchController" for the view container that holds the controls to link;
    there can only be one KickSwitchController for each view container

    -	the verifyView( ) function gets called once per child view of the outer container; this is where we save the control

    -	the valueChanged( ) function gets called when *any* of the view container's sub-controls are moved

    \author Steve Dwyer
    \remark Based on the KnobLinkController by Will Pirkle http://www.willpirkle.com
    \version Revision : 1.0
    \date Date : 2021-12-24
    */
    class KickSwitchController : public IController
    {
    public:
        /** KickSwitchController constructor
        \param _parentController - pointer to the parent controller so we can forward messages to it
        */
        KickSwitchController(IController* _parentController)
        {
            // --- save the parent listener
            parentController = _parentController;
        }

        ~KickSwitchController() override = default;

        /** called once per child view of the container that owns the sub-controller this is where we grab and store the
            view objects that we want to link
            \param view - the newly created view to use
            \param attributes - UIAttributes of control
            \param description - IUIDescription of control
            \return the verified view
        */
        CView* verifyView(CView* view, const UIAttributes& attributes, const IUIDescription* description) override
        {
            auto* kickSwitch = dynamic_cast<CKickButton*>(view);
            auto* led = dynamic_cast<COnOffButton*>(view);

            // --- save button, push back knob onto list
            if (kickSwitch)
            {
                activator = kickSwitch;
            }
            else if (led)
            {
                indicator = led;
            }

            return view;
        }

        /** called when any control  in the view container changes; we only care about the link button and the knobs, we ignore the others
        view objects that we want to link
        \param control - the control whose value changed
        */
        void valueChanged(CControl* control) override
        {
            // --- set the link flag
            if (control == activator)
            {
                // On Mouse Down *only*, hence when control's value normalized = 1
                if (isEqual(control->getValueNormalized(), 1.0f))
                {
                    // --- set the control visually and flip it's value (arithmetic negate)
                    indicator->setValueNormalized(indicator->getValueNormalized() * -1 + 1);

                    // --- do the value change at parent level, to set on plugin
                    parentController->valueChanged(indicator);
                    // Force a repaint
                    indicator->invalid();
                }
            }

            // --- do the value change at parent level, to set on plugin
            parentController->valueChanged(control);
        }

        /** called once per child view of the container - we simply forward the call to the parent listener
        \param attributes - UIAttributes of control
        \param description - IUIDescription of control
        \return the verified view
        */
        CView* createView(const UIAttributes& attributes, const IUIDescription* description) override
        {
            return parentController->createView(attributes, description);
        }

        /** called when the user begins to edit a control (mouse click) - we simply forward the call to the parent listener
        \param pControl - the control
        */
        void controlBeginEdit(CControl* pControl) override { parentController->controlBeginEdit(pControl); }

        /** called when the user ends editing a control (mouse released) - we simply forward the call to the parent listener
        \param pControl - the control
        */
        void controlEndEdit(CControl* pControl) override { parentController->controlEndEdit(pControl); }

        /** register the control update receiver objects that
            allow us to ultimately bind GUI controls to plugin variables (thread-safe of course)
        \param pControl - the control
        */
        void controlTagWillChange(CControl* pControl) override
        {
            pControl->setListener(parentController);
            parentController->controlTagWillChange(pControl);
            pControl->setListener(this);
        }

        /** register the control update receiver objects that
        allow us to ultimately bind GUI controls to plugin variables (thread-safe of course)
        \param pControl - the control
        */
        void controlTagDidChange(CControl* pControl) override
        {
            pControl->setListener(parentController);
            parentController->controlTagDidChange(pControl);
            pControl->setListener(this);
        }

    protected:
        // --- the parent controller; we can issue IController commands to it!
        IController* parentController = nullptr; ///< pointer to owning listener

        // The Kick Switch
        CKickButton* activator = nullptr;

        // The On/Off Status LED
        COnOffButton* indicator = nullptr;
    };
    
    /**
    \class KnobLabelPair
    \author Steve Dwyer
    \version Revision : 1.0
    \date Date : 2021-12-27
    */
    class KnobLabelPair
    {
    public:
        /** KnobLabelPair constructor (default) */
        KnobLabelPair() = default;
        ~KnobLabelPair() = default;

        // The Knob
        CAnimKnob* knob = nullptr;

        // The TextLabel
        CTextEdit* text = nullptr;
    };

    /**
    \class VariableToolTipKnobController
    \ingroup Custom-SubControllers
    \brief
    In this object, you can add multiple knob controls so that moving a control moves updates the tooltip text to
    display the knob's value.

    -	Sub-Controller that manages some number of CAnimKnobs

    -	use the sub-controller string "VariableToolTipKnobController" for the view container that holds the knobs;
    there can only be one VariableToolTipKnobController for each view container

    -	the verifyView( ) function gets called once per child view of the outer
    container; this is where push the CAnimKnob controls onto our vector

    -	the valueChanged( ) function gets called when *any* of the view containers sub
    controls are moved; we only stash and control the CAnimKnobs so you can mix
    other controls in the same view container that won't be affected

    \author Steve Dwyer
    \remark Based on the KnobLinkController by Will Pirkle http://www.willpirkle.com
    \version Revision : 1.0
    \date Date : 2010-12-26
    */
    class VariableToolTipKnobController : public IController
    {
    public:
        /** VariableToolTipKnobController constructor
        \param _parentController - pointer to the parent controller so we can forward messages to it
        */
        VariableToolTipKnobController(IController* _parentController)
        {
            // --- save the parent listener
            parentController = _parentController;
        }

        ~VariableToolTipKnobController() override
        {
            knobLabelPairs.clear();
        }

        /** called once per child view of the container that owns the sub-controller this is where we grab and store the
            view objects that we want to manage
            \param view - the newly created view to use
            \param attributes - UIAttributes of control
            \param description - IUIDescription of control
            \return the verified view
        */
        CView* verifyView(CView* view, const UIAttributes& attributes, const IUIDescription* description) override
        {
            auto* container = dynamic_cast<CViewContainer*>(view);

            // --- push back knob onto list
            if (container)
            {
                auto* knobLabelPair = new KnobLabelPair();
                knobLabelPair->knob = nullptr;
                knobLabelPair->text = nullptr;

                container->forEachChild([&] (CView* view)
                {
                    auto* knob = dynamic_cast<CAnimKnob*>(view);
                    if (knob)
                    {
                        knobLabelPair->knob = knob;
                    }
                    
                    auto* text = dynamic_cast<CTextEdit*>(view);
                    if (text)
                    {
                        knobLabelPair->text = text;
                    }
                });

                if (knobLabelPair->knob && knobLabelPair->text)
                {
                    knobLabelPair->knob->setTooltipText(knobLabelPair->text->getText());
                    knobLabelPairs.push_back(knobLabelPair);
                }
                else
                {
                    delete knobLabelPair;
                }
            }

            return view;
        }

        /** called when any control in the view container changes; we only care about the link button and the knobs, we ignore the others
	    view objects that we want to link
	    \param control - the control whose value changed
	    */
        void valueChanged(CControl* control) override
        {
            const auto knobLabelPair = getKnob(control);
            // --- make sure this is not a rogue control
            if (knobLabelPair)
            {
                auto knob = knobLabelPair->knob;
                const auto text = knobLabelPair->text;

                // --- do the value change at parent level, to set on plugin
                // Vital that this is done before setting the tooltip text, as it will update the CTextEdit object's text attribute
                parentController->valueChanged(knob);
                knob->setTooltipText(text->getText());

                // Force re-paint
                knob->invalid();
            }
            // --- do the value change at parent level, to set on plugin
            parentController->valueChanged(control);
        }

        /** called once per child view of the container - we simply forward the call to the parent listener
        \param attributes - UIAttributes of control
        \param description - IUIDescription of control
        \return the verified view
        */
        CView* createView(const UIAttributes& attributes, const IUIDescription* description) override
        {
            return parentController->createView(attributes, description);
        }

        /** called when the user begins to edit a control (mouse click) - we simply forward the call to the parent listener
        \param pControl - the control
        */
        void controlBeginEdit(CControl* pControl) override { parentController->controlBeginEdit(pControl); }

        /** called when the user ends editing a control (mouse released) - we simply forward the call to the parent listener
        \param pControl - the control
        */
        void controlEndEdit(CControl* pControl) override { parentController->controlEndEdit(pControl); }

        /** register the control update receiver objects that
            allow us to ultimately bind GUI controls to plugin variables (thread-safe of course)
        \param pControl - the control
        */
        void controlTagWillChange(CControl* pControl) override
        {
            pControl->setListener(parentController);
            parentController->controlTagWillChange(pControl);
            pControl->setListener(this);
        }

        /** register the control update receiver objects that
        allow us to ultimately bind GUI controls to plugin variables (thread-safe of course)
        \param pControl - the control
        */
        void controlTagDidChange(CControl* pControl) override
        {
            pControl->setListener(parentController);
            parentController->controlTagDidChange(pControl);
            pControl->setListener(this);
        }

    protected:
        // --- the parent controller; we can issue IController commands to it!
        IController* parentController = nullptr; ///< pointer to owning listener

        // All managed knobs, which will update their tooltip text when their value changes
        typedef std::vector<KnobLabelPair*> KnobList;
        KnobList knobLabelPairs;

    private:
        /** test to see if control is in the list of controls
        \param control - control to test
        \return true if control is in list, false otherwise
        */
        KnobLabelPair* getKnob(CControl* control)
        {
            KnobLabelPair* returnValue = nullptr;

            for (KnobLabelPair* pair : knobLabelPairs)
            {
                if (pair->knob == control)
                {
                    returnValue = pair;
                    break;
                }
            }

            return returnValue;
        }
    };


    /**
    \class SimpleValueVariableToolTipKnobController
    \ingroup Custom-SubControllers
    \brief
    In this object, you can add multiple knob controls so that moving a control moves updates the tooltip text to
    display the knob's value.

    -	Sub-Controller that manages some number of CAnimKnobs

    -	use the sub-controller string "SimpleValueVariableToolTipKnobController" for the view container that holds the knobs;
    there can only be one SimpleValueVariableToolTipKnobController for each view container

    -	the verifyView( ) function gets called once per child view of the outer
    container; this is where push the CAnimKnob controls onto our vector

    -	the valueChanged( ) function gets called when *any* of the view containers sub
    controls are moved; we only stash and control the CAnimKnobs so you can mix
    other controls in the same view container that won't be affected

    \author Steve Dwyer
    \remark Based on the KnobLinkController by Will Pirkle http://www.willpirkle.com
    \version Revision : 1.0
    \date Date : 2010-12-28
    */
    class SimpleValueVariableToolTipKnobController : public IController
    {
    public:
        /** SimpleValueVariableToolTipKnobController constructor
        \param _parentController - pointer to the parent controller so we can forward messages to it
        \param _multiplicationFactor - the factor by which to multiply the displayed value in the tooltip text (used e.g. for "Goes to Eleven" knobs)
        */
        SimpleValueVariableToolTipKnobController(IController* _parentController, float _multiplicationFactor)
        {
            // Store the multiplication factor, to display knob values with (used e.g. for "Goes to Eleven" knobs)
            multiplicationFactor = _multiplicationFactor;
            // --- save the parent listener
            parentController = _parentController;
        }

        /** SimpleValueVariableToolTipKnobController constructor
        \param _parentController - pointer to the parent controller so we can forward messages to it
        */
        SimpleValueVariableToolTipKnobController(IController* _parentController)
        {
            // Store the multiplication factor, to display knob values with (used e.g. for "Goes to Eleven" knobs)
            multiplicationFactor = KNOB_FACTOR_DEFAULT;
            // --- save the parent listener
            parentController = _parentController;
        }

        ~SimpleValueVariableToolTipKnobController() override
        {
            knobs.clear();
        }

        /** called once per child view of the container that owns the sub-controller this is where we grab and store the
            view objects that we want to manage
            \param view - the newly created view to use
            \param attributes - UIAttributes of control
            \param description - IUIDescription of control
            \return the verified view
        */
        CView* verifyView(CView* view, const UIAttributes& attributes, const IUIDescription* description) override
        {
            auto* knob = dynamic_cast<CAnimKnob*>(view);

            // --- push back knob onto list
            if (knob)
            {
                const auto tooltipText = getTooltipText(knob->getValueNormalized(), multiplicationFactor, PRECISION_2_DP);
                knob->setTooltipText(tooltipText.c_str());
                knobs.push_back(knob);
            }

            return view;
        }

        /** called when any control in the view container changes; we only care about the link button and the knobs, we ignore the others
	    view objects that we want to link
	    \param control - the control whose value changed
	    */
        void valueChanged(CControl* control) override
        {
            const auto knob = getKnob(control);
            // --- make sure this is not a rogue control
            if (knob)
            {
                // --- do the value change at parent level, to set on plugin
                // Vital that this is done before setting the tooltip text, as it will update the CTextEdit object's text attribute
                parentController->valueChanged(knob);
                const auto tooltipText = getTooltipText(knob->getValueNormalized(), multiplicationFactor, PRECISION_2_DP);
                knob->setTooltipText(tooltipText.c_str());

                // Force re-paint
                knob->invalid();
            }
            // --- do the value change at parent level, to set on plugin
            parentController->valueChanged(control);
        }

        /** called once per child view of the container - we simply forward the call to the parent listener
        \param attributes - UIAttributes of control
        \param description - IUIDescription of control
        \return the verified view
        */
        CView* createView(const UIAttributes& attributes, const IUIDescription* description) override
        {
            return parentController->createView(attributes, description);
        }

        /** called when the user begins to edit a control (mouse click) - we simply forward the call to the parent listener
        \param pControl - the control
        */
        void controlBeginEdit(CControl* pControl) override { parentController->controlBeginEdit(pControl); }

        /** called when the user ends editing a control (mouse released) - we simply forward the call to the parent listener
        \param pControl - the control
        */
        void controlEndEdit(CControl* pControl) override { parentController->controlEndEdit(pControl); }

        /** register the control update receiver objects that
            allow us to ultimately bind GUI controls to plugin variables (thread-safe of course)
        \param pControl - the control
        */
        void controlTagWillChange(CControl* pControl) override
        {
            pControl->setListener(parentController);
            parentController->controlTagWillChange(pControl);
            pControl->setListener(this);
        }

        /** register the control update receiver objects that
        allow us to ultimately bind GUI controls to plugin variables (thread-safe of course)
        \param pControl - the control
        */
        void controlTagDidChange(CControl* pControl) override
        {
            pControl->setListener(parentController);
            parentController->controlTagDidChange(pControl);
            pControl->setListener(this);
        }

    protected:
        // --- the parent controller; we can issue IController commands to it!
        IController* parentController = nullptr; ///< pointer to owning listener

        // All managed knobs, which will update their tooltip text when their value changes
        typedef std::vector<CAnimKnob*> KnobList;
        KnobList knobs;

    private:
        // Default to 2 d.p. for Tooltip texts
        const int PRECISION_2_DP = 2;

        // The multiplication factor, to display knob values with (used e.g. for "Goes to Eleven" knobs)
        float multiplicationFactor;

        static std::string getTooltipText(const float value, const float multiplicationFactor, const int precision)
        {
            // Create an output string stream
            std::ostringstream stream;
            // Set Fixed-Point Notation
            stream << std::fixed;
            // Set precision to 2 digits
            stream << std::setprecision(precision);
            //Add float to stream
            stream << value * multiplicationFactor;
            // Get string from output string stream
            return stream.str();
        }


        /** test to see if control is in the list of controls
        \param control - control to test
        \return true if control is in list, false otherwise
        */
        CAnimKnob* getKnob(CControl* control)
        {
            CAnimKnob* returnValue = nullptr;

            for (CAnimKnob* knob : knobs)
            {
                if (knob == control)
                {
                    returnValue = knob;
                    break;
                }
            }

            return returnValue;
        }
    };
}
