/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   not_comp_3.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 19:36:33 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/16 19:37:43 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Warlock.hpp"

int main(void)
{
    Warlock richard("Richard", "Mistress of Magma");
    Warlock bob("Bob", "Mistress of Magma");
    bob = richard;

    return 0;
}
    