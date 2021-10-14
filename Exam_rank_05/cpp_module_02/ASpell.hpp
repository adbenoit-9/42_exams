/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASpell.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 21:37:49 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/14 22:18:29 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASPELL_HPP
# define ASPELL_HPP

# include <string>
# include <iostream>
# include "ATarget.hpp"

class ATarget;

class ASpell
{
	private:
		std::string name;
		std::string effects;

	public:
		ASpell();
		ASpell(const std::string &, const std::string &);
		ASpell(const ASpell &);
		virtual ~ASpell();
		ASpell&	operator=(const ASpell &);
		
		const std::string&	getName() const;
		const std::string&	getEffects() const;
		virtual ASpell*		clone() const = 0;
		void				launch(const ATarget &) const;
};

#endif
