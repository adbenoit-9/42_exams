/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TargetGenerator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adbenoit <adbenoit@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 17:38:14 by adbenoit          #+#    #+#             */
/*   Updated: 2021/10/14 23:48:13 by adbenoit         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TARGETGENERATOR_HPP
# define TARGETGENERATOR_HPP

# include "ATarget.hpp"
# include <vector>

class TargetGenerator
{
    private:
        std::vector<ATarget *> generator;
        
        TargetGenerator(const TargetGenerator &);
        TargetGenerator& operator= (const TargetGenerator &);
    
    public:
        TargetGenerator();
        ~TargetGenerator();

        void        learnTargetType(ATarget *);
        void        forgetTargetType(const std::string &);
        ATarget*    createTarget(const std::string &);
};

#endif
