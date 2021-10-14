/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SpellBook.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 17:38:14 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/14 23:48:36 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPELLBOOK_HPP
# define SPELLBOOK_HPP

# include "ASpell.hpp"
# include <vector>

class SpellBook
{
    private:
        std::vector<ASpell *> book;
        
        SpellBook(const SpellBook &);
        SpellBook& operator= (const SpellBook &);
    
    public:
        SpellBook();
        ~SpellBook();

        void    learnSpell(ASpell *);
        void    forgetSpell(const std::string &);
        ASpell* createSpell(const std::string &);
};

#endif
