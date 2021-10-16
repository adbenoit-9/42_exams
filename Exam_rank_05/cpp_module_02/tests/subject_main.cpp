/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   subject_main.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 18:13:58 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/16 20:23:16 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Warlock.hpp"
#include "../Fwoosh.hpp"
#include "../Dummy.hpp"
#include "../BrickWall.hpp"
#include "../Polymorph.hpp"
#include "../TargetGenerator.hpp"
#include "../Fireball.hpp"

int main(void)
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

  return 0;
}
