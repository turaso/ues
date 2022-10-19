#include "markerformwidget.h"

//#include <Wt/WDoubleValidator.h> // ???
#include <Wt/WHBoxLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WVBoxLayout.h>

#include <chrono>
#include <regex>
#include <string>

MarkerFormWidget::MarkerFormWidget () : Wt::WContainerWidget ()
{
  auto mainLayout = std::make_unique<Wt::WVBoxLayout> ();

  auto make_pair_label_and_input =
      [&mainLayout]<typename T> (T, const std::string label) { // ! memory (T)
        auto hLayout
            = mainLayout->addLayout (std::make_unique<Wt::WHBoxLayout> ());
        hLayout->addWidget (std::make_unique<Wt::WLabel> (label));
        return hLayout->addWidget (std::make_unique<T> ());
      }; // ?! update with reflection

  this->titleEdit_ = make_pair_label_and_input (Wt::WLineEdit (), "Title");

  this->xEdit_ = make_pair_label_and_input (Wt::WDoubleSpinBox (), "Latitude");
  this->xEdit_->setMinimum (Event::xMin);
  this->xEdit_->setMaximum (Event::xMax);

  this->yEdit_
      = make_pair_label_and_input (Wt::WDoubleSpinBox (), "Longitude");
  this->yEdit_->setMinimum (Event::yMin);
  this->yEdit_->setMaximum (Event::yMax);

  this->startDateEdit_
      = make_pair_label_and_input (Wt::WDateEdit (), "Start date");
  this->startDateEdit_->setBottom (
      Wt::WDate (std::chrono::system_clock::now ()));
  this->startTimeEdit_
      = make_pair_label_and_input (Wt::WTimeEdit (), "Start time");
  this->startTimeEdit_->setBottom (
      this->startDateEdit_->date ()
              == Wt::WDate (std::chrono::system_clock::now ())
          ? Wt::WTime (std::chrono::hours ().count (),
                       std::chrono::minutes ().count (),
                       std::chrono::seconds ().count (),
                       std::chrono::milliseconds ().count ())
          : Wt::WTime (0, 0));

  this->endEditCheck_ = std::make_unique<Wt::WCheckBox> ("Known end time");
  this->endEditCheck_->setChecked (true);
  this->endDateEdit_
      = make_pair_label_and_input (Wt::WDateEdit (), "End time");
  this->endDateEdit_->setBottom (
      Wt::WDate (std::chrono::system_clock::now ()));
  this->endTimeEdit_
      = make_pair_label_and_input (Wt::WTimeEdit (), "End time");
  this->endTimeEdit_->setBottom (
      this->endDateEdit_->date ()
              == Wt::WDate (std::chrono::system_clock::now ())
          ? Wt::WTime (std::chrono::hours ().count (),
                       std::chrono::minutes ().count (),
                       std::chrono::seconds ().count (),
                       std::chrono::milliseconds ().count ())
          : Wt::WTime (0, 0)); // ! there is similar code above
  this->endEditCheck_->changed ().connect ([this] () {
    this->endDateEdit_->setEnabled (this->endEditCheck_->checkState ()
                                    == Wt::CheckState::Checked);
    this->endTimeEdit_->setEnabled (this->endEditCheck_->checkState ()
                                    == Wt::CheckState::Checked);
  }); // ! check

  this->participantsLimitEdit_ = make_pair_label_and_input (
      Wt::WSpinBox (), "Participants limit number");
  this->participantsLimitEdit_->setMinimum (Event::participantsLimitMin);
  this->participantLimitButtonGroup_
      = std::make_unique<Wt::WButtonGroup> ().get ();
  auto participantsLimitGroupBox = std::make_unique<Wt::WGroupBox> ();
  auto buttonNoLimit = participantsLimitGroupBox->addWidget (
      std::make_unique<Wt::WRadioButton> ("No limit"));
  this->participantLimitButtonGroup_->addButton (
      buttonNoLimit, static_cast<int> (ParticipantsLimit::NO_LIMIT));
  auto buttonUnknown = participantsLimitGroupBox->addWidget (
      std::make_unique<Wt::WRadioButton> ("Unknown"));
  this->participantLimitButtonGroup_->addButton (
      buttonUnknown, static_cast<int> (ParticipantsLimit::UNKNOWN));
  auto buttonWithLimit = participantsLimitGroupBox->addWidget (
      std::make_unique<Wt::WRadioButton> ("With limit"));
  this->participantLimitButtonGroup_->addButton (
      buttonWithLimit, static_cast<int> (ParticipantsLimit::FIXED));
  this->participantLimitButtonGroup_->checkedChanged ().connect (
      [this, &buttonWithLimit] (Wt::WRadioButton *button) {
        this->participantsLimitEdit_->setEnabled (button == buttonWithLimit);
      });

  mainLayout->addWidget (std::make_unique<Wt::WLabel> ("Description"));
  this->descriptionEdit_
      = mainLayout->addWidget (std::make_unique<Wt::WTextEdit> (), 1);

  this->saveButton_
      = mainLayout->addWidget (std::make_unique<Wt::WPushButton> ("Save"));
  this->saveButton_->clicked ().connect ([this] () {
    if (this->validate ())
      {
        this->emitSave ();
      }
    else
      {
        this->showError ();
      }
  });

  this->setLayout (std::move (mainLayout));
}

MarkerFormWidget::MarkerFormWidget (
    Wt::WLeafletMap::Coordinate coords) // !! check
    : MarkerFormWidget ()
{
  this->xEdit_->setText (std::to_string (coords.latitude ()));
  this->yEdit_->setText (std::to_string (coords.longitude ()));
}

MarkerFormWidget::MarkerFormWidget (Event &event) // !! check
    : MarkerFormWidget ({ event.x, event.y })
{
  this->titleEdit_->setText (event.title);
  this->startDateEdit_->setDate (event.datetimeStart.date ());
  this->startTimeEdit_->setTime (event.datetimeStart.time ());
  auto setEndDateTimeEnabled = [this] (bool enabled) {
    this->endEditCheck_->setEnabled (!enabled);
    this->endDateEdit_->setEnabled (enabled);
    this->endTimeEdit_->setEnabled (enabled);
  };
  if (event.datetimeFinish.isNull ())
    {
      setEndDateTimeEnabled (false);
    }
  else
    {
      setEndDateTimeEnabled (true);
      this->endDateEdit_->setDate (event.datetimeFinish.date ());
      this->endTimeEdit_->setTime (event.datetimeFinish.time ());
    }
  this->participantLimitButtonGroup_->setSelectedButtonIndex (
      event.participantsLimit >= 2
          ? static_cast<int> (ParticipantsLimit::FIXED)
          : event.participantsLimit);
  if (event.participantsLimit >= static_cast<int> (ParticipantsLimit::FIXED))
    {
      this->participantLimitButtonGroup_->setSelectedButtonIndex (
          static_cast<int> (ParticipantsLimit::FIXED));
      this->participantsLimitEdit_->setEnabled (true);
      this->participantsLimitEdit_->setValue (event.participantsLimit);
    }
  else
    {
      this->participantLimitButtonGroup_->setSelectedButtonIndex (
          event.participantsLimit);
      this->participantsLimitEdit_->setEnabled (false);
    }
  this->descriptionEdit_->setText (event.description);
}

bool
MarkerFormWidget::validate () const
{
  return this->validateTitleEdit ()
         && this->validateDescriptionEdit (); // ? more "useless" checks
}

bool
MarkerFormWidget::validateTitleEdit () const
{
  return std::regex_match (this->titleEdit_->text ().toUTF8 (),
                           std::regex ("(\\w+)")); // ! check regex
}

bool
MarkerFormWidget::validateXEdit () const
{
  return true;
}

bool
MarkerFormWidget::validateYEdit () const
{
  return true;
}

bool
MarkerFormWidget::validateStartDateEdit () const
{
  return true;
}

bool
MarkerFormWidget::validateStartTimeEdit () const
{
  return true;
}

bool
MarkerFormWidget::validateEndDateEdit () const
{
  return true;
}

bool
MarkerFormWidget::validateEndTimeEdit () const
{
  return true;
}

bool
MarkerFormWidget::validateParticipantsLimitEdit () const
{
  return true;
}

bool
MarkerFormWidget::validateDescriptionEdit () const
{
  return this->descriptionEdit_->text ().toUTF8 ().size ()
         <= Event::descriptionMaxLength;
}

void
MarkerFormWidget::emitSave ()
{
  auto event = std::make_unique<Event> ();
  event->title = this->titleEdit_->text ().toUTF8 ();
  event->x = this->xEdit_->value ();
  event->y = this->yEdit_->value ();
  event->datetimeStart = Wt::WDateTime (this->startDateEdit_->date (),
                                        this->startTimeEdit_->time ());
  event->datetimeFinish = this->endEditCheck_->isChecked ()
                              ? Wt::WDateTime (this->endDateEdit_->date (),
                                               this->endTimeEdit_->time ())
                              : Wt::WDateTime ();
  event->participantsLimit
      = this->participantLimitButtonGroup_->checkedId ()
                == static_cast<int> (ParticipantsLimit::FIXED)
            ? this->participantsLimitEdit_->value ()
            : this->participantLimitButtonGroup_->checkedId ();
  // ? event->participants
  event->description = this->descriptionEdit_->text ().toUTF8 ();
  this->saveSignal.emit (*event.get ());
}

void
MarkerFormWidget::showError ()
{
  this->saveButton_->setText ("Error. Check and try again"); // !!
}
