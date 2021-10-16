/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   not_comp_0.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 19:38:48 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/16 20:16:32 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Warlock.hpp"

void    isConst(std::string& str) {
    std::cout << str;
}

int main(void)
{
    Warlock richard("Richard", "Mistress of Magma");
    isConst(richard.getName());
    
    return 0;
}
