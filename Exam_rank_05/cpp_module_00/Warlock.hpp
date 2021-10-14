/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Warlock.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 17:38:14 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/14 17:58:28 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WARLOCK_HPP
# define WARLOCK_HPP

# include <string>
# include <iostream>

class Warlock
{
    private:
        std::string name;    
        std::string title;
        
        Warlock();
        Warlock(const Warlock &);
        Warlock& operator= (const Warlock &);
    
    public:
        Warlock(const std::string &, const std::string &);
        ~Warlock();

        const std::string&  getName() const;
        const std::string&  getTitle() const;
        void                setTitle(const std::string &);
        void                introduce() const;
};

#endif
