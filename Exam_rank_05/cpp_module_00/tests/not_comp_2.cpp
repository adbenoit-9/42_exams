/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   not_comp_2.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 19:37:53 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/16 19:38:07 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Warlock.hpp"

int main(void)
{
    Warlock richard("Richard", "Mistress of Magma");
    Warlock bob(richard);

    return 0;
}
