/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Warlock.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 17:38:10 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/14 23:47:09 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Warlock.hpp"

Warlock::Warlock(const std::string &n, const std::string &t) :
name(n), title(t) {
    std::cout << this->name << ": This looks like another boring day." << std::endl;
}

Warlock::~Warlock() {
    std::cout << this->name << ": My job here is done!" << std::endl;
}

const std::string&  Warlock::getName() const {
    return this->name;
}
const std::string&  Warlock::getTitle() const {
    return this->title;
}
void    Warlock::setTitle(const std::string &t) {
    this->title = t;
}

void    Warlock::introduce() const {
    std::cout << this->name << ": I am " << this->name << ", "
            << this->title << "!" << std::endl;
}

void    Warlock::learnSpell(ASpell *spell){
    this->book.learnSpell(spell);
}

void    Warlock::forgetSpell(std::string name) {
    this->book.forgetSpell(name);
}

void    Warlock::launchSpell(std::string name, const ATarget &target) {
    return this->book.createSpell(name)->launch(target);
}

#include "Polymorph.hpp"
#include "BrickWall.hpp"
#include "TargetGenerator.hpp"
#include "Fireball.hpp"

int main()
{
  Warlock richard("Richard", "foo");
  richard.setTitle("Hello, I'm Richard the Warlock!");
  BrickWall model1;

  Polymorph* polymorph = new Polymorph();
  TargetGenerator tarGen;

  tarGen.learnTargetType(&model1);
  richard.learnSpell(polymorph);

  Fireball* fireball = new Fireball();

  richard.learnSpell(fireball);

  ATarget* wall = tarGen.createTarget("Inconspicuous Red-brick Wall");

  richard.introduce();
  richard.launchSpell("Polymorph", *wall);
  richard.launchSpell("Fireball", *wall);
}
