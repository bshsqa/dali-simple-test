/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/atspi-interfaces/action.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-integ.h>
#include <dali/dali.h>

#include <cstdint>
#include <iostream>
#include <string>

namespace
{
using Dali::Toolkit::Control;
namespace DevelControl = Dali::Toolkit::DevelControl;

uint32_t ReadingMask()
{
  using Dali::Integration::Accessibility::ReadingInfoType;

  return (1u << static_cast<uint32_t>(ReadingInfoType::NAME)) |
         (1u << static_cast<uint32_t>(ReadingInfoType::ROLE)) |
         (1u << static_cast<uint32_t>(ReadingInfoType::DESCRIPTION)) |
         (1u << static_cast<uint32_t>(ReadingInfoType::STATE));
}

class MockAtspiClient
{
public:
  void Dump(const std::string& label, Control control)
  {
    auto* accessible = Dali::Accessibility::Accessible::Get(control);
    if(!accessible)
    {
      std::cout << label << ": no accessible object\n";
      return;
    }

    std::cout << label << '\n';
    std::cout << "  name: " << accessible->GetName() << '\n';
    std::cout << "  description: " << accessible->GetDescription() << '\n';
    std::cout << "  value: " << accessible->GetValue() << '\n';
    std::cout << "  role: " << static_cast<uint32_t>(accessible->GetRole()) << '\n';
    std::cout << "  states: 0x" << std::hex << accessible->GetStates().GetRawData64() << std::dec << '\n';
    std::cout << "  hidden: " << (accessible->IsHidden() ? "true" : "false") << '\n';
    std::cout << "  relations: " << accessible->GetRelationSet().size() << '\n';
    std::cout << "  reading-info: 0x" << std::hex << DevelControl::GetAccessibilityReadingInfoTypeRaw(control) << std::dec << '\n';

    const auto attributes = accessible->GetAttributes();
    for(const auto& attribute : attributes)
    {
      std::cout << "  attribute: " << attribute.first << '=' << attribute.second << '\n';
    }

    auto action = accessible->GetFeature<Dali::Accessibility::Action>();
    if(action)
    {
      std::cout << "  actions: " << action->GetActionCount() << '\n';
      for(std::size_t index = 0u; index < action->GetActionCount(); ++index)
      {
        std::cout << "    [" << index << "] " << action->GetActionName(index) << '\n';
      }
    }
  }

  void Activate(Control control)
  {
    auto* accessible = Dali::Accessibility::Accessible::Get(control);
    if(!accessible)
    {
      return;
    }

    auto action = accessible->GetFeature<Dali::Accessibility::Action>();
    if(action)
    {
      const bool result = action->DoAction("activate");
      std::cout << "mock activate result: " << (result ? "true" : "false") << '\n';
    }
  }
};

class AccessibilitySmoke : public Dali::ConnectionTracker
{
public:
  explicit AccessibilitySmoke(Dali::Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AccessibilitySmoke::OnInit);
  }

private:
  void OnInit(Dali::Application application)
  {
    auto window = application.GetWindow();
    window.SetBackgroundColor(Dali::Color::WHITE);

    auto title = Dali::Toolkit::TextLabel::New("Accessibility smoke");
    title.SetProperty(Dali::Actor::Property::NAME, "Title");
    title.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, Dali::ParentOrigin::TOP_LEFT);
    title.SetProperty(Dali::Actor::Property::PIVOT, Dali::Pivot::TOP_LEFT);
    title.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector2(40.0f, 40.0f));
    title.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(480.0f, 64.0f));
    title.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::BLACK);
    title.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Toolkit::Accessibility::Role::TEXT);
    title.SetProperty(DevelControl::Property::ACCESSIBILITY_NAME, "Accessibility smoke title");
    window.Add(title);

    auto button = Dali::Toolkit::PushButton::New();
    button.SetProperty(Dali::Toolkit::Button::Property::LABEL, "Activate");
    button.SetProperty(Dali::Actor::Property::NAME, "PrimaryAction");
    button.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, Dali::ParentOrigin::TOP_LEFT);
    button.SetProperty(Dali::Actor::Property::PIVOT, Dali::Pivot::TOP_LEFT);
    button.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector2(40.0f, 140.0f));
    button.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(240.0f, 80.0f));
    button.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Toolkit::Accessibility::Role::BUTTON);
    button.SetProperty(DevelControl::Property::ACCESSIBILITY_NAME, "Primary action");
    button.SetProperty(DevelControl::Property::ACCESSIBILITY_DESCRIPTION, "Triggers the mock AT-SPI action path");
    button.SetProperty(DevelControl::Property::AUTOMATION_ID, "primary-action");
    button.SetAccessibilityState(Dali::Toolkit::Accessibility::State::ENABLED, true);
    DevelControl::AppendAccessibilityAttribute(button, "automationId", "primary-action");
    DevelControl::AppendAccessibilityRelation(button, title, Dali::Toolkit::Accessibility::RelationType::LABELLED_BY);
    DevelControl::SetAccessibilityReadingInfoTypeRaw(button, ReadingMask());
    DevelControl::AccessibilityActivateSignal(button).Connect(this, &AccessibilitySmoke::OnActivated);
    window.Add(button);

    auto check = Dali::Toolkit::CheckBoxButton::New();
    check.SetProperty(Dali::Toolkit::Button::Property::LABEL, "Selected state");
    check.SetProperty(Dali::Actor::Property::NAME, "SelectedState");
    check.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, Dali::ParentOrigin::TOP_LEFT);
    check.SetProperty(Dali::Actor::Property::PIVOT, Dali::Pivot::TOP_LEFT);
    check.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector2(40.0f, 250.0f));
    check.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(320.0f, 80.0f));
    check.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Toolkit::Accessibility::Role::CHECK_BOX);
    check.SetProperty(DevelControl::Property::ACCESSIBILITY_NAME, "Selected state option");
    check.SetAccessibilityState(Dali::Toolkit::Accessibility::State::ENABLED, true);
    check.SetAccessibilityState(Dali::Toolkit::Accessibility::State::CHECKED, true);
    DevelControl::AppendAccessibilityRelation(check, title, Dali::Toolkit::Accessibility::RelationType::LABELLED_BY);
    DevelControl::SetAccessibilityReadingInfoTypeRaw(check, ReadingMask());
    window.Add(check);

    auto progress = Dali::Toolkit::TextLabel::New("Progress 60%");
    progress.SetProperty(Dali::Actor::Property::NAME, "Progress");
    progress.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, Dali::ParentOrigin::TOP_LEFT);
    progress.SetProperty(Dali::Actor::Property::PIVOT, Dali::Pivot::TOP_LEFT);
    progress.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector2(40.0f, 360.0f));
    progress.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(320.0f, 64.0f));
    progress.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::BLACK);
    progress.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Toolkit::Accessibility::Role::PROGRESS_BAR);
    progress.SetProperty(DevelControl::Property::ACCESSIBILITY_NAME, "Loading progress");
    progress.SetProperty(DevelControl::Property::ACCESSIBILITY_DESCRIPTION, "Progress value exposed to accessibility clients");
    progress.SetProperty(DevelControl::Property::ACCESSIBILITY_VALUE, "60%");
    progress.SetAccessibilityState(Dali::Toolkit::Accessibility::State::ENABLED, true);
    progress.SetAccessibilityState(Dali::Toolkit::Accessibility::State::BUSY, true);
    DevelControl::SetAccessibilityReadingInfoTypeRaw(progress, ReadingMask());
    window.Add(progress);

    auto dialog = Dali::Toolkit::TextLabel::New("Modal dialog");
    dialog.SetProperty(Dali::Actor::Property::NAME, "Dialog");
    dialog.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, Dali::ParentOrigin::TOP_LEFT);
    dialog.SetProperty(Dali::Actor::Property::PIVOT, Dali::Pivot::TOP_LEFT);
    dialog.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector2(400.0f, 140.0f));
    dialog.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(280.0f, 80.0f));
    dialog.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::BLACK);
    dialog.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Toolkit::Accessibility::Role::DIALOG);
    dialog.SetProperty(DevelControl::Property::ACCESSIBILITY_NAME, "Modal dialog");
    dialog.SetProperty(DevelControl::Property::ACCESSIBILITY_DESCRIPTION, "Modal object exposed through accessibility metadata");
    dialog.SetProperty(DevelControl::Property::ACCESSIBILITY_IS_MODAL, true);
    dialog.SetAccessibilityState(Dali::Toolkit::Accessibility::State::ENABLED, true);
    DevelControl::SetAccessibilityReadingInfoTypeRaw(dialog, ReadingMask());
    window.Add(dialog);

    auto list = Dali::Toolkit::TextLabel::New("Scrollable list");
    list.SetProperty(Dali::Actor::Property::NAME, "ScrollableList");
    list.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, Dali::ParentOrigin::TOP_LEFT);
    list.SetProperty(Dali::Actor::Property::PIVOT, Dali::Pivot::TOP_LEFT);
    list.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector2(400.0f, 250.0f));
    list.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(280.0f, 80.0f));
    list.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::BLACK);
    list.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Toolkit::Accessibility::Role::LIST);
    list.SetProperty(DevelControl::Property::ACCESSIBILITY_NAME, "Scrollable list");
    list.SetProperty(DevelControl::Property::ACCESSIBILITY_DESCRIPTION, "Scrollable flag exposed to accessibility clients");
    list.SetProperty(DevelControl::Property::ACCESSIBILITY_SCROLLABLE, true);
    list.SetAccessibilityState(Dali::Toolkit::Accessibility::State::ENABLED, true);
    DevelControl::SetAccessibilityReadingInfoTypeRaw(list, ReadingMask());
    window.Add(list);

    auto hidden = Dali::Toolkit::TextLabel::New("Hidden notification");
    hidden.SetProperty(Dali::Actor::Property::NAME, "HiddenNotification");
    hidden.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, Dali::ParentOrigin::TOP_LEFT);
    hidden.SetProperty(Dali::Actor::Property::PIVOT, Dali::Pivot::TOP_LEFT);
    hidden.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector2(400.0f, 360.0f));
    hidden.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(280.0f, 64.0f));
    hidden.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR, Dali::Color::BLACK);
    hidden.SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Toolkit::Accessibility::Role::NOTIFICATION);
    hidden.SetProperty(DevelControl::Property::ACCESSIBILITY_NAME, "Hidden notification");
    hidden.SetProperty(DevelControl::Property::ACCESSIBILITY_DESCRIPTION, "Hidden accessibility object for mock inspection");
    hidden.SetProperty(DevelControl::Property::ACCESSIBILITY_HIDDEN, true);
    hidden.SetAccessibilityState(Dali::Toolkit::Accessibility::State::ENABLED, true);
    DevelControl::SetAccessibilityReadingInfoTypeRaw(hidden, ReadingMask());
    window.Add(hidden);

    mClient.Dump("button before action", button);
    mClient.Activate(button);
    mClient.Dump("check box", check);
    mClient.Dump("progress", progress);
    mClient.Dump("dialog", dialog);
    mClient.Dump("scrollable list", list);
    mClient.Dump("hidden notification", hidden);

    auto timer = Dali::Timer::New(250u);
    timer.TickSignal().Connect(this, &AccessibilitySmoke::Quit);
    timer.Start();
  }

  void OnActivated()
  {
    std::cout << "activate signal emitted\n";
  }

  bool Quit()
  {
    mApplication.Quit();
    return false;
  }

  Dali::Application& mApplication;
  MockAtspiClient    mClient;
};
} // namespace

int main(int argc, char** argv)
{
  auto app = Dali::Application::New(&argc, &argv);
  AccessibilitySmoke smoke(app);
  app.MainLoop();
  return 0;
}
