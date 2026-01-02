/**
 * @file gui.cc
 * @author LEMAIRE Alice 0.3, CAPDEVILLE Maud 0.7
 * @version
 */

#include "gui.h"

#include <cairomm/context.h>
#include <glibmm.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/label.h>

#include <iostream>

const Cairo::RefPtr<Cairo::Context> *Environnement::ptcr = nullptr;

Environnement::Environnement(Simulation &simulation) : simulation(simulation)
{
  set_draw_func(sigc::mem_fun(*this, &Environnement::on_draw));
}

Environnement::~Environnement() {}

void Environnement::on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int w, int h)
{
  monde.on_draw_world(cr, w, h);
  monde.on_draw_limite(cr, w, h);
  ptcr = &cr;
  simulation.draw_simulation(monde, cr, w, h);
}

MyEvent::MyEvent(Simulation &simulation)
    : simulation(simulation),
      environnement(simulation),
      timer_added(false),
      disconnect(false),
      timeout_value(delta_t * 1000),
      m_main_box(Gtk::Orientation::HORIZONTAL, 2),
      m_widgets(Gtk::Orientation::VERTICAL, 2),
      m_environment(Gtk::Orientation::VERTICAL, 2),
      m_General(Gtk::Orientation::VERTICAL, 2),
      m_info(Gtk::Orientation::VERTICAL, 2),
      m_maj(Gtk::Orientation::HORIZONTAL, 2),
      m_Button_exit("exit"),
      m_Button_open("open"),
      m_Button_save("save"),
      m_Button_start("start"),
      m_Button_step("step"),
      m_checkbutton_seaweed("Naissance d'algue")

{
  m_Button_start.signal_clicked().connect(
      sigc::mem_fun(*this, &MyEvent::on_button_clicked_StartOrStop));
  m_Button_exit.signal_clicked().connect(
      sigc::mem_fun(*this, &MyEvent::on_button_clicked_exit));
  m_Button_step.signal_clicked().connect(
      sigc::mem_fun(*this, &MyEvent::on_button_clicked_step));
  m_Button_save.signal_clicked().connect(
      sigc::mem_fun(*this, &MyEvent::on_button_clicked_save));
  m_Button_open.signal_clicked().connect(
      sigc::mem_fun(*this, &MyEvent::on_button_clicked_open));
  m_checkbutton_seaweed.signal_toggled().connect(
      sigc::mem_fun(*this, &MyEvent::on_checkbutton_toggled));

  auto controller = Gtk::EventControllerKey::create();
  controller->signal_key_pressed().connect(
      sigc::mem_fun(*this, &MyEvent::on_window_key_pressed), false);
  add_controller(controller);

  set_resizable(true);
  set_hexpand(false);
  set_vexpand(false);
  set_child(m_main_box);
  set_title("");
  set_default_size(taille_dessin + 200, taille_dessin);

  m_label_general.set_text("General");
  m_widgets.append(m_label_general);
  m_label_general.set_halign(m_widgets.get_halign());

  m_widgets.append(m_General);
  m_widgets.append(s1);

  m_label_info.set_text("Info: nombre de ...");
  m_widgets.append(m_label_info);
  m_widgets.append(m_info);

  m_General.append(m_Button_exit);
  m_General.append(m_Button_open);
  m_General.append(m_Button_save);
  m_General.append(m_Button_start);
  m_General.append(m_Button_step);
  m_General.append(m_checkbutton_seaweed);

  m_label_maj.set_text("Mise à jour: ");
  m_maj.append(m_label_maj);
  m_maj.append(m_label_maj_data);
  m_info.append(m_maj);

  m_label_algues.set_text("algues: ");
  m_algues.append(m_label_algues);
  m_algues.append(m_label_algues_data);
  m_info.append(m_algues);

  m_label_corails.set_text("corails: ");
  m_corails.append(m_label_corails);
  m_corails.append(m_label_corails_data);
  m_info.append(m_corails);

  m_label_scavenger.set_text("charognards: ");
  m_charognards.append(m_label_scavenger);
  m_charognards.append(m_label_scavenger_data);
  m_info.append(m_charognards);

  m_widgets.set_size_request(200, taille_dessin);

  m_main_box.set_margin(10);
  m_main_box.append(m_widgets);
  m_main_box.append(s2);
  m_main_box.append(m_environment);

  m_environment.set_size_request(taille_dessin, taille_dessin);
  m_environment.append(environnement);
  environnement.set_expand();

  set_infos();
}

void MyEvent ::on_button_clicked_StartOrStop()
{
  if (m_Button_start.get_label() == "start")
  {
    if (not timer_added)
    {
      sigc::slot<bool()> my_slot =
          sigc::bind(sigc::mem_fun(*this, &MyEvent::on_timeout));

      // This is where we connect the slot to the Glib::signal_timeout()
      auto conn = Glib::signal_timeout().connect(my_slot, timeout_value);

      timer_added = true;

      std::cout << "Timer added" << std::endl;
    }
    else
    {
      std::cout << "The timer already exists : nothing more is created" << std::endl;
    }
    m_Button_start.set_label("stop");
  }
  else
  {
    if (not timer_added)
    {
      std::cout << "Sorry, there is no active timer at the moment." << std::endl;
    }
    else
    {
      std::cout << "manually disconnecting the timer " << std::endl;
      disconnect = true;
      timer_added = false;
    }
    m_Button_start.set_label("start");
  }
}

void MyEvent ::on_button_clicked_exit()
{
  hide();
}

void MyEvent::on_button_clicked_save()
{
  auto dialog = new Gtk::FileChooserDialog("Please choose a file",
                                           Gtk::FileChooser::Action::SAVE);
  dialog->set_transient_for(*this);
  dialog->set_modal(true);
  dialog->signal_response().connect(sigc::bind(
      sigc::mem_fun(*this, &MyEvent::on_file_dialog_response_save), dialog));

  dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
  dialog->add_button("_Save", Gtk::ResponseType::OK);

  auto filter_text = Gtk::FileFilter::create();
  filter_text->set_name("Text files");
  filter_text->add_mime_type("text/plain");
  dialog->add_filter(filter_text);

  auto filter_cpp = Gtk::FileFilter::create();
  filter_cpp->set_name("C/C++ files");
  filter_cpp->add_mime_type("text/x-c");
  filter_cpp->add_mime_type("text/x-c++");
  filter_cpp->add_mime_type("text/x-c-header");
  dialog->add_filter(filter_cpp);

  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name("Any files");
  filter_any->add_pattern("*");
  dialog->add_filter(filter_any);

  dialog->show();
}

void MyEvent::on_button_clicked_open()
{
  disconnect = false;
  auto dialog = new Gtk::FileChooserDialog("Please choose a file",
                                           Gtk::FileChooser::Action::OPEN);
  dialog->set_transient_for(*this);
  dialog->set_modal(true);
  dialog->signal_response().connect(sigc::bind(
      sigc::mem_fun(*this, &MyEvent::on_file_dialog_response_open), dialog));

  dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
  dialog->add_button("_Open", Gtk::ResponseType::OK);

  auto filter_text = Gtk::FileFilter::create();
  filter_text->set_name("Text files");
  filter_text->add_mime_type("text/plain");
  dialog->add_filter(filter_text);

  auto filter_cpp = Gtk::FileFilter::create();
  filter_cpp->set_name("C/C++ files");
  filter_cpp->add_mime_type("text/x-c");
  filter_cpp->add_mime_type("text/x-c++");
  filter_cpp->add_mime_type("text/x-c-header");
  dialog->add_filter(filter_cpp);

  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name("Any files");
  filter_any->add_pattern("*");
  dialog->add_filter(filter_any);

  dialog->show();
}

void MyEvent ::on_button_clicked_step()
{
  if (!timer_added)
  {
    on_timeout();
  }
  else
  {
    on_button_clicked_StartOrStop();
  }
}

bool MyEvent::on_timeout()
{
  if (disconnect)
  {
    disconnect = false; // reset for next time a Timer is created

    return false; // End of Timer
  }

  // simulation.increment_number_update();
  // cout << "Simulation number: " << simulation.get_number_update() << endl;
  simulation.update(checkbutton_seaweed);
  // création d'une algue

  environnement.queue_draw();
  set_infos();

  return true;
}

bool MyEvent::on_window_key_pressed(guint keyval, guint, Gdk::ModifierType state)
{
  switch (gdk_keyval_to_unicode(keyval))
  {
  case 's':
    on_button_clicked_StartOrStop();
    return true;
  case '1':
    on_button_clicked_step();
    return true;
  }
  return false;
}

void MyEvent::on_file_dialog_response_open(int response_id,
                                           Gtk::FileChooserDialog *dialog)
{
  switch (response_id)
  {
  case Gtk::ResponseType::OK:
  {
    std::cout << "Open clicked." << std::endl;

    auto filename = dialog->get_file()->get_path();
    std::cout << "File selected: " << filename << std::endl;

    simulation = Simulation{};
    if (simulation.read_contain_invalid_data(const_cast<char *>(filename.c_str())))
      simulation.destroy_lifeforms();

    environnement.queue_draw();
    set_infos();

    break;
  }
  case Gtk::ResponseType::CANCEL:
  {
    std::cout << "Cancel clicked." << std::endl;
    break;
  }
  default:
  {
    std::cout << "Unexpected button clicked." << std::endl;
    break;
  }
  }
  delete dialog;
}

void MyEvent::on_file_dialog_response_save(int response_id,
                                           Gtk::FileChooserDialog *dialog)
{
  switch (response_id)
  {
  case Gtk::ResponseType::OK:
  {
    auto filename = dialog->get_file()->get_path();
    std::cout << "File selected: " << filename << std::endl;

    std::ofstream myfile;
    myfile.open(filename);

    simulation.save(myfile);
  }
  case Gtk::ResponseType::CANCEL:
  {
    std::cout << "Cancel clicked." << std::endl;
    break;
  }
  default:
  {
    std::cout << "Unexpected button clicked." << std::endl;
    break;
  }
  }
  delete dialog;
}

void MyEvent ::set_infos()
{
  m_label_maj_data.set_text(
      std::to_string(simulation.get_number_update())); // display he simulation clock
  m_label_algues_data.set_text(std::to_string(simulation.get_number_algue()));
  m_label_corails_data.set_text(std::to_string(simulation.get_number_coral()));
  m_label_scavenger_data.set_text(std::to_string(simulation.get_number_scavenger()));
}

void MyEvent ::on_checkbutton_toggled() { checkbutton_seaweed = !checkbutton_seaweed; }
